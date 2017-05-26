#include <msgpack.hpp>                                   
#include <string>                                      
#include <cstring>                                     
#include <iostream>                                    
using namespace std;                                   
                                                       
int main(int argc, char** argv)                        
{                                                      
                                                       
    msgpack::sbuffer  sbuf;                            
    msgpack::packer<msgpack::sbuffer>   pker(&sbuf);   
                                                       
    // ���л�                                          
    pker.pack_map(3);                                  
    pker.pack(string("type"));                         
    pker.pack(3);                                      
    pker.pack(string("ratio"));                        
    pker.pack(2.15);                                   
    pker.pack(string("msg"));                          
    pker.pack(string("hello world"));                  
                                                       
    // �����л�                                        
    msgpack::unpacked  unpack;                         
    msgpack::unpack(&unpack, sbuf.data(), sbuf.size());
                                                       
    // ֱ��������                                    
    msgpack::object  obj = unpack.get();               
    cout << obj << endl;                               
                                                       
    // ���ʾ����ֵ��                                  
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