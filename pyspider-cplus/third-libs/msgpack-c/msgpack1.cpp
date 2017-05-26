#include <msgpack.hpp>                                   
#include <string>                                      
#include <cstring>                                     
#include <iostream>                                    
using namespace std;                                   
                                                       
int main(int argc, char** argv)                        
{                                                      
                                                       
    msgpack::sbuffer  sbuf;                            
    msgpack::packer<msgpack::sbuffer>   pker(&sbuf);   
                                                       
    // 序列化                                          
    pker.pack_map(3);                                  
    pker.pack(string("type"));                         
    pker.pack(3);                                      
    pker.pack(string("ratio"));                        
    pker.pack(2.15);                                   
    pker.pack(string("msg"));                          
    pker.pack(string("hello world"));                  
                                                       
    // 反序列化                                        
    msgpack::unpacked  unpack;                         
    msgpack::unpack(&unpack, sbuf.data(), sbuf.size());
                                                       
    // 直接输出结果                                    
    msgpack::object  obj = unpack.get();               
    cout << obj << endl;                               
                                                       
    // 访问具体键值对                                  
    msgpack::object_kv*  pkv;                          
    msgpack::object_kv*  pkv_end;                      
    msgpack::object      pk, pv;                       
    if(obj.via.map.size > 0)                           
    {                                                  
        pkv = obj.via.map.ptr;                         
        pkv_end = obj.via.map.ptr + obj.via.map.size;  
                                                       
        do                                             
        {                                              
            pk = pkv->key;                             
            pv = pkv->val;                             
                                                       
            cout << pk << ", " << pv << endl;          
                                                       
            ++pkv;                                     
        }                                              
        while (pkv < pkv_end);                         
    }                                                  
                                                       
    return 0;                                          
}                                                      