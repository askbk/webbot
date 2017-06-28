#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <string>
#include "curlpp/cURLpp.hpp"
#include "curlpp/Options.hpp"

using namespace std;
using namespace curlpp

queue<string> que;
set<string> crawled;

void crawl(){
	ofstream outFile;
	string URL = que.front();
	que.pop();
	outFile.open("crawled.txt", ios::ate);
}

int main(){
	ofstream outFile;
	string startURL;
	cout << "Input URL to start crawl:\n";
	cin >> startURL;
	que.push(startURL);

	while(que.empty()!){
		crawl();
	}

	Cleanup myCleanup;
	ostrinstream os;
	os << options::Url(startURL);
	cout << "\n" <<  os.str(); << "\n";
}
