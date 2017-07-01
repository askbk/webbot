#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <string>
//#include <curlpp/include/curlpp/cURLpp.hpp>
//#include <curlpp/include/curlpp/Options.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>


using namespace std;
using namespace boost::asio::ip;
//using namespace curlpp;

//initialize( long flags = CURL_GLOBAL_ALL );

queue<string> que;
set<string> crawled;

void crawl(){
	ofstream outFile;
	string URL = que.front();
	que.pop();
	outFile.open("crawled.txt", ios::ate);
	
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(URL, "daytime");
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::socket socket(io_service);
	boost::asio::connect(socket, endpoint_iterator);

	while(true){
		boost::array<char, 128> buf;
		boost::system::error_code error;
		
		size_t len = socket.read_some(boost::asio::buffer(buf), error);

		if(error == boost::asio::error::eof) break;
		else if(error) throw boost::system::system_error(error);

		cout.write(buf.data(), len);
	}

}

int main(){
	string startURL;
	cout << "Input URL to start crawl:\n";
	cin >> startURL;
	que.push(startURL);

	while(!que.empty()){
		crawl();
	}

	//Cleanup myCleanup;
	//ostrinstream os;
	//os << options::Url(startURL);
	//cout << "\n" <<  os.str(); << "\n";
}
