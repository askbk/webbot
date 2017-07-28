#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <regex>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml++/libxml++.h>


using namespace std;
using namespace boost::asio::ip;

queue< pair<string, string > > que;
set< pair<string, string> > crawled;

pair<string, string> parseUrl(string inputURL){
    string host, query = "/";
    regex exrp( "^(?:http://)?([^/]+)(?:/?.*/?)/(.*)$" );
    match_results<string::const_iterator> what;
    if( regex_search( inputURL, what, exrp ) ) {
        host = what[1];
        query.append(what[2]);
        //cout << "what[1].first: " << what[1] << "\n"; 
    }
    return make_pair(host, query);
}

void crawl(){
    //ofstream outFile;
    pair<string, string> URL = que.front();
    que.pop();
    string host = URL.first;
    string path = URL.second;
    //outFile.open("crawled.txt", ios::app | ios::out);
    
    try{
        boost::asio::io_service io_service;

        // get list of endpoints
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(host, "http");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // try each endpoint
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        //form request
        boost::asio::streambuf request;
        ostream request_stream(&request);
        request_stream << "GET " << path << " HTTP/1.1\r\n";
        request_stream << "Host: " << host << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // send request
        boost::asio::write(socket, request);

        //read reqponse
        boost::asio::streambuf response;
        boost::asio::read_until(socket,response,"\r\n");

        // check ok
        istream response_stream(&response);
        string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        string status_message;
        getline(response_stream, status_message);
        if(!response_stream || http_version.substr(0,5) != "HTTP/"){
            cout << "inv response\n";
            return;
        }
        if(status_code != 200){
            cout << "response ret with stat code " << status_code << "\n";
            return;
        }
        
        //read resp headers
        boost::asio::read_until(socket, response, "\r\n\r\n");

        //process headers
        string header;
        while(getline(response_stream, header) && header != "\r"){
            cout << header << "\n";
        }
        cout << "\n";

        // string to hold html and input
        string htmlResponse;
        string input;
        // ostringstream ss;

        
        //write content out
        if(response.size() > 0){ //cout << &response;
            // htmlResponse.append(&response);
            //response_stream >> input;
            //htmlResponse.append(input);
            ostringstream ss;
            ss << &response;
 ;           input = ss.str();
            htmlResponse.append(input);
        }

        //read until EOF
        boost::system::error_code error;
        while(boost::asio::read(socket,response,boost::asio::transfer_at_least(1),error)){
            //cout << &response;
            //response_stream >> input;
            //htmlResponse.append(input);
            ostringstream ss;
            ss << &response;
            input = ss.str();
            htmlResponse.append(input);

        }
        if(error != boost::asio::error::eof){
            throw boost::system::system_error(error);
        }
        //outFile << URL.first << URL.second << "\n";
        //outFile.close();
        
        // Parse HTML and create a DOM tree
        xmlDoc* doc = htmlReadDoc((xmlChar*)htmlResponse.c_str(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
 
        // Encapsulate raw libxml document in a libxml++ wrapper
        xmlNode* r = xmlDocGetRootElement(doc);
        xmlpp::Element* root = new xmlpp::Element(r);
 
        // Grab the IP address
        string xpath = "//a/@href";
        auto elements = root->find(xpath);
        //cout << dynamic_cast<xmlpp::ContentNode*>(elements[0])->get_content() << std::endl;
 
        delete root;
        xmlFreeDoc(doc);
    }
    catch (exception& e){
        cout << "exception: " << e.what() << "\n";
    }
}

int main(){
    string startURL;
    cout << "Input URL to start crawl:\n";
    cin >> startURL;
    
    que.push(parseUrl(startURL));

    crawl();
    
    
}
