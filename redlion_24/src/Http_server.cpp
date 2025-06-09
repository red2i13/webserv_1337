#include "../includes/Http_server.hpp"

Http_server::Http_server(){
    server_names.push_back("site.local");
    port.push_back("8080");

}
Http_server::~Http_server(){

}
Http_server::Http_server(const Http_server &obj){

}
Http_server & Http_server::operator=(const Http_server &obj){
    
}
