#include <iostream>
#include <cstdio>  
#include <list>
#include <string>
#include <iterator>
#include <map>

using namespace std;

int main(int argc, char **argv)
{
  map<string,double> map_strings;

  map_strings["apples"]=5;
  map_strings["watermelons"]=7;
  map_strings["pears"]=1;
  map_strings["blackberry"]=6;
 
  map<string,double>::iterator p;
  
for(p=map_strings.begin(); p!=map_strings.end();) {
  string str = p->first;
  double value = p->second;
  if(value > 5) {
    cout << "erased value " << value << " index "<<str << endl;
    p = map_strings.erase(p);
  }
  else{
    cout << "kept index " << str << " value "<<value << endl;        
    ++p;
    //cout << "kept index " << str << " value "<<value << endl;
  }
  cout << "index 2" << str << " value "<<value << endl; 
}
cout << "map size: " << map_strings.size() << endl;

 return (0);
}
