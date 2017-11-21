


gcc -fPIC -shared great_module.c -o great_module.so -I/usr/include/python2.7/ -lpython2.7


>>> from great_module import great_function 
>>> great_function(2)
3








1.包裹函数_great_function。它负责将Python的参数转化为C的参数（PyArg_ParseTuple），调用实际的great_function，并处理great_function的返回值，最终返回给Python环境。

2.导出表GreateModuleMethods。
它负责告诉Python这个模块里有哪些函数可以被Python调用。
导出表的名字可以随便起，每一项有4个参数：
第一个参数是提供给Python环境的函数名称，
第二个参数是_great_function，即包裹函数。
第三个参数的含义是参数变长，
第四个参数是一个说明性的字符串。
导出表总是以{NULL, NULL, 0, NULL}结束。

3.导出函数initgreat_module。这个的名字不是任取的，是你的module名称添加前缀init。导出函数中将模块名称与导出表进行连接。

