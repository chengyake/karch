//#include <lua.hpp>
//g++ -o test lua_call_c_as_string_code.c -I/usr/include/lua5.2/ -llua5.2

//#include <lua.h>
//gcc -o test lua_call_c_as_string_code.c -I/usr/include/lua5.2/ -llua5.2


#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

//��Lua���õ�Cע�ắ����
static int add2(lua_State* L)
{
    //���ջ�еĲ����Ƿ�Ϸ���1��ʾLua����ʱ�ĵ�һ������(������)���������ơ�
    //���Lua�����ڵ���ʱ���ݵĲ�����Ϊnumber���ú�����������ֹ�����ִ�С�
    double op1 = luaL_checknumber(L,1);
    double op2 = luaL_checknumber(L,2);
    //�������Ľ��ѹ��ջ�С�����ж������ֵ��������������ѹ��ջ�С�
    lua_pushnumber(L,op1 + op2);
    //����ֵ������ʾ��C�����ķ���ֵ��������ѹ��ջ�еķ���ֵ������
    return 1;
}

//��һ����Lua���õ�Cע�ắ����
static int sub2(lua_State* L)
{
    double op1 = luaL_checknumber(L,1);
    double op2 = luaL_checknumber(L,2);
    lua_pushnumber(L,op1 - op2);
    return 1;
}

const char* testfunc = "print(add2(1.0,2.0)) print(sub2(20.1,19))";

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    //��ָ���ĺ���ע��ΪLua��ȫ�ֺ������������е�һ���ַ�������ΪLua����
    //�ڵ���C����ʱʹ�õ�ȫ�ֺ��������ڶ�������Ϊʵ��C������ָ�롣
    lua_register(L, "add2", add2);
    lua_register(L, "sub2", sub2);
    //��ע�������е�C����֮�󣬼�����Lua�Ĵ������ʹ����Щ�Ѿ�ע���C�����ˡ�
    if (luaL_dostring(L,testfunc))
        printf("Failed to invoke.\n");
    lua_close(L);
    return 0;
}


