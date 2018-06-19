import struct
f = open('out.raw','rb')
d_str = f.read()
f.close()

d_len = len(d_str)
data = struct.unpack(str(d_len/4)+'f',d_str)

print(data)
