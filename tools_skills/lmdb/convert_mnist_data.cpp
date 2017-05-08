// 解析caffe中proto类型文件的头文件
#include "caffe/proto/caffe.pb.h"

using namespace caffe; // NOLINT(build/namespace)
using std::string;

// GFLAGS 工具定义命令行选项 backend, 默认值为 lmdb, 即: --backend=lmdb
DEFINE_string(backend, "lmdb", "The backend for storing the result");

// 大小端转换, MNIST 原始数据文件中 32 位整型值为大端存储, C/C++ 变量为小端存储，因此需要加入转换机制
uint32_t swap_endian(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

// 转换数据集函数
void convert_dataset(const char* image_filename, const char* label_filename,
        const char* db_path, const string& db_backend) {

    // 用 C++ 输入文件流以二进制方式打开
    // 定义, 打开图像文件 对象: image_file（读入的文件名, 读入方式）, 此处以二进制的方式
    std::ifstream image_file(image_filename, std::ios::in | std::ios::binary);
    // 定义, 打开标签文件 对象: label_file（读入的文件名, 读入方式）, 此处以二进制的方式
    std::ifstream label_file(label_filename, std::ios::in | std::ios::binary);

    // CHECK: 用于检测文件能否正常打开函数
    CHECK(image_file) << "Unable to open file " << image_filename;
    CHECK(label_file) << "Unable to open file " << label_filename;

    // 读取魔数与基本信息
    // uint32_t 用 typedef 来自定义的一种数据类型, unsigned int32, 每个int32整数占用4个字节, 这样做是为了程序的可扩展性
    uint32_t magic; // 魔数
    uint32_t num_items; // 文件包含条目总数 
    uint32_t num_labels; // 标签值
    uint32_t rows; // 行数
    uint32_t cols; // 列数

    // 读取魔数: magic
    // image_file.read( 读取内容的指针, 读取的字节数 ) , magic 是一个 int32 类型的整数，每个占 4 个字节，所以这里指定为 4
    // reinterpret_cast 为 C++ 中定义的强制转换符, 这里把 &magic, 即 magic 的地址（一个 16 进制的数）, 转变成 char 类型的指针
    image_file.read(reinterpret_cast(&magic), 4);
    // 大端到小端的转换
    magic = swap_endian(magic);
    // 校验图像文件中魔数是否为 2051, 不是则报错
    CHECK_EQ(magic, 2051) << "Incorrect image file magic.";
    // 同理, 校验标签文件中的魔数是否为 2049, 不是则报错
    label_file.read(reinterpret_cast(&magic), 4);
    magic = swap_endian(magic);
    CHECK_EQ(magic, 2049) << "Incorrect label file magic.";

    // 读取图片的数量: num_items
    image_file.read(reinterpret_cast(&num_items), 4);
    num_items = swap_endian(num_items); // 大端到小端转换
    // 读取图片标签的数量
    label_file.read(reinterpret_cast(&num_labels), 4);
    num_labels = swap_endian(num_labels); // 大端到小端转换
    // 图片数量应等于其标签数量, 检查两者是否相等
    CHECK_EQ(num_items, num_labels);
    // 读取图片的行大小
    image_file.read(reinterpret_cast(&rows), 4);
    rows = swap_endian(rows); // 大端到小端转换
    // 读取图片的列大小
    image_file.read(reinterpret_cast(&cols), 4);
    cols = swap_endian(cols); // 大端到小端转换

    // lmdb 相关句柄
    MDB_env *mdb_env;
    MDB_dbi mdb_dbi;
    MDB_val mdb_key, mdb_data;
    MDB_txn *mdb_txn;
    // leveldb 相关句柄
    leveldb::DB* db;
    leveldb::Options options;
    options.error_if_exists = true;
    options.create_if_missing = true;
    options.write_buffer_size = 268435456;
    level::WriteBatch* batch = NULL;

    // 打开 db
    if (db_backend == "leveldb") { // leveldb
        LOG(INFO) << "Opening leveldb " << db_path;
        leveldb::Status status = leveldb::DB::Open(
                options, db_path, &db);
        CHECK(status.ok()) << "Failed to open leveldb " << db_path << ". Is it already existing?";
        batch = new leveldb::WriteBatch();
    }else if (db_backend == "lmdb") { // lmdb
        LOG(INFO) << "Opening lmdb " << db_path;
        CHECK_EQ(mkdir(db_path, 0744), 0) << "mkdir " << db_path << "failed";
        CHECK_EQ(mdb_env_create(&mdb_env), MDB_SUCCESS) << "mdb_env_create failed";
        CHECK_EQ(mdb_env_set_mapsize(mdb_env, 1099511627776), MDB_SUCCESS) << "mdb_env_set_mapsize failed"; // 1TB
        CHECK_EQ(mdb_env_open(mdb_env, db_path, 0, 0664), MDB_SUCCESS) << "mdb_env_open_failed";
        CHECK_EQ(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn), MDB_SUCCESS) << "mdb_txn_begin failed";
        CHECK_EQ(mdb_open(mdb_txn, NULL, 0, &mdb_dbi), MDB_SUCCESS) << "mdb_open failed. Does the lmdb already exist?";
    } else {
        LOG(FATAL) << "Unknown db backend " << db_backend;
    }

    // 将读取数据保存至 db
    char label;
    char* pixels = new char[rows * cols];
    int count = 0;
    const int kMaxKeyLength = 10;
    char key_cstr[kMaxKeyLength];
    string value;

    // 设置datum数据对象的结构，其结构和源图像结构相同
    Datum datum;
    datum.set_channels(1);
    datum.set_height(rows);
    datum.set_width(cols);

    // 输出 Log, 输出图片总数
    LOG(INFO) << "A total of " << num_items << " items.";
    // 输出 Log, 输出图片的行、列大小
    LOG(INFO) << "Rows: " << rows << " Cols: " << cols;

    // 读取图片数据以及 label 存入 protobuf 定义好的数据结构中,
    // 序列化成字符串储存到数据库中,
    // 这里为了减少单次操作带来的带宽成本（验证数据包完整等）, 
    // 每 1000 次执行一次操作
    for (int item_id = 0; item_id < num_items; ++item_id) {
        // 从数据中读取 rows * cols 个字节, 图像中一个像素值（应该是 int8 类型）用一个字节表示即可
        image_file.read(pixels, rows * cols);
        // 读取标签
        label_file.read(&label, 1);
        // set_data 函数, 把源图像值放入 datum 对象
        datum.set_data(pixels, rows*cols);
        // set_label 函数, 把标签值放入 datum
        datum.set_label(label);

        // snprintf(str1, size_t, "format", str), 把 str 按照 format 的格式以字符串的形式写入 str1, size_t 表示写入的字符个数    
        // 这里是把 item_id 转换成 8 位长度的十进制整数，然后在变成字符串复制给 key_str, 如：item_id=1500（int）, 则 key_cstr = 00015000（string, \0为字符串结束标志）
        snprintf(key_cstr, kMaxKeyLength, "%08d", item_id);
        datum.SerializeToString(&value);
        // 感觉是将 datum 中的值序列化成字符串，保存在变量 value 内，通过指针来给 value 赋值
        string keystr(key_cstr);

        // 放到数据库中
        if (db_backend == "leveldb") { // leveldb
            // 通过 batch 中的子方法 Put, 把数据写入 datum 中（此时在内存中）
            batch->Put(keystr, value);
        } else if (db_backend == "lmdb") { // lmdb
            // mv 应该是 move value, 应该是和 write() 和 read() 函数文件读写的方式一样, 以固定的子节长度按照地址进行读写操作
            // 获取 value 的子节长度, 类似 sizeof() 函数
            mdb_data.mv_size = value.size()
                // 把 value 的首个字符地址转换成空类型的指针
                mdb_data.mv_data = reinterpret_cast(&value[0]);
            mdb_key.mv_size = keystr.size();
            mdb_key.mv_data = reinterpret_cast(&keystr[0]);
            // 通过 mdb_put 函数把 mdb_key 和 mdb_data 所指向的数据,  写入到 mdb_dbi
            CHECK_EQ(mdb_put(mdb_txn, mdb_dbi, &mdb_key, &mdb_data, 0), MDB_SUCCESS) << "mdb_put failed";
        } else {
            LOG(FATAL) << "Unknown db backend " << db_back_end;
        }

        // 把 db 数据写入硬盘
        // 选择 1000 个样本放入一个 batch 中，通过 batch 以批量的方式把数据写入硬盘
        // 写入硬盘通过 db.write（） 函数来实现
        if (++count % 1000 == 0) {
            // 批量提交更改
            if(db_backend == "leveldb") { // leveldb
                // 把batch写入到 db 中，然后删除 batch 并重新创建
                db->Write(leveldb::WriteOptions(), batch);
                delete batch;
                batch = new leveldb::WriteBatch();
            } else if (db_backend == "lmdb") { // lmdb
                // 通过 mdb_txn_commit 函数把 mdb_txn 数据写入到硬盘
                CHECK_EQ(mdb_txn_commit(mdb_txn), MDB_SUCCESS) << "mdb_txn_commit failed";
                // 重新设置 mdb_txn 的写入位置, 追加（继续）写入
                CHECK_EQ(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn), MDB_SUCCESS) << "mdb_txn_begin failed";
            } else {
                LOG(FATAL) << "Unknown db backend " << db_backend;
            }
        } // if (++count % 1000 == 0) 

    } // for (int item_id = 0; item_id < num_items; ++item_id)

    // 写最后一个 batch 
    if (count % 1000 != 0) {
        if (db_backend == "leveldb") { // leveldb
            db->Write(leveldb::WriteOptions(), batch);
            delete batch;
            delete db; // 删除临时变量，清理内存占用
        } else if (db_backend == "lmdb") { // lmdb
            CHECK_EQ(mdb_txn_commit(mdb_txn), MDB_SUCCESS) << "mdb_txn_commit failed";
            // 关闭 mdb 数据对象变量
            mdb_close(mdb_env, mdb_dbi);
            // 关闭 mdb 操作环境变量
            mdb_env_close(mdb_env);
        } else {
            LOG(FATAL) << "Unknown db backend " << db_backend;
        }
        LOG(ERROE) << "Processed " << count << " files.";
    }

    delete[] pixels;
} // void convert_dataset(const char* image_filename, const char* label_filename, const char* db_path, const string& db_backend)

int main(int argc, char** argv) {
#ifndef GFLAGS_GFLAGS_H
    namespace gflags = google;
#endif

    gflags::SetUsageMessage("This script converts the MNIST dataset to \n"
            "the lmdb/leveldb format used by Caffe to load data. \n"
            "Usage:\n"
            "    convert_mnist_data [FLAGS] input_image_file input_label_file "
            "output_db_file\n"
            "The MNIST dataset could be downloaded at\n"
            "    http://yann.lecun.com/exdb/mnist/\n"
            "You should gunzip them after downloading,"
            "or directly use the data/mnist/get_mnist.sh\n");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // FLAGS_backend 在前面通过 DEFINE_string 定义，是字符串类型
    const string& db_backend = FLAGS_backend;

    if (argc != 4) {
        gflags::ShowUsageWithFlagsRestrict(argv[0], "examples/mnist/convert_mnist_data");
    } else {
        google::InitGoogleLogging(argv[0]);
        convert_dataset(argv[1], argv[2], argv[3], db_backend);
    }

    return 0;
}
