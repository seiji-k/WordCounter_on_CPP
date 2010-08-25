//============================================================================
// Name        : text_edit.cpp
// Author      : seiji-k
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <time.h>


using namespace std;
typedef map<string, int> StrIntMap; //string(単語)とint(頻度)を格納するmap。                                                             //mapは第一要素で自動的にソートされる。
                                    //->firstで第一要素(index) ->secondで二要素(value)
typedef pair<string, string> ngram_Pair;
typedef map<ngram_Pair, int> ngram_Count;

//-----自作lowercase関数--------------------------------
string Lowercase(string s){
  int n = 'a' - 'A';
  for (int i = 0; i < s.length(); i++) {
    // 大文字(A-Z)なら変換
    if ('A'<=s[i] && s[i]<='Z') {
      s[i] = s[i] + n;
      //     	cout << "out"<<endl;
    }
  }
  return s;
  //	cout << "out"<<endl;
}




//--------自作trim関数。記号を取り除く。-------------------
string Trim(string s) {
  for(int i = 0; i <= s.length(); i++ ){
    if(s[i] == '!'||s[i] == '"'||s[i] == '#'||s[i] == '$'||
       s[i] == '%'||s[i] == '&'||s[i] == '('||s[i] == ')'||
       s[i] == '*'||s[i] == '+'||s[i] == ','||s[i] == '.'||
       s[i] == '/'||s[i] == ':'||s[i] == ';'||s[i] == '<'||
       s[i] == '='||s[i] == '>'||s[i] == '?'||s[i] == '['||
       s[i] == ']'||s[i] == '^'||s[i] == '_'||s[i] == '{'||
       s[i] == '|'||s[i] == '}'||s[i] == '~'||s[i] == '`' 
       ){
      s.erase(i,1);
      i--;    
    }
  }
  // cout << s << endl;
  return s;
}



//---------------fの全単語をmapに格納------------------
void CountWords(istream& f, StrIntMap& words,int* total){

  string s;

  while (f >> s){         //(f >> s)で空白を飛ばして一単語ずつ読んでくれる
    s = Lowercase (s);     //自作lowercaseを使うときはこっち。
    s = Trim(s);         //;.,[]()とかを掃除
    //transform(s.begin(),s.end(),s.begin(),::tolower);//標準lowercase。gccではなぜかtolowerではなく 
    //::tolowerにしなければいけないそう。。
    ++(*total);
    ++words[s];          
  }
}

//-------------挿入ソートを用い、wの頻度上位単語freq個をnumber_of_word[]に格納-
void InsertSort(StrIntMap& w,   StrIntMap::iterator number_of_word[], int freq){
 
  StrIntMap::iterator p = w.begin();//wの先頭freq個の単語をnumber_of_wordsに格納
  for(int i = 0 ; i<freq ; i++){    //初期化がわり。   
    number_of_word[i] = p;
    p++;
  }

  for (StrIntMap::iterator p = w.begin(); p != w.end(); ++p){
    int s = p->second;                     //sはpのさす単語の頻度。
	
    for(int i = 0; i < freq; i++){        //頻度ランキングを順番に。
      if(s > number_of_word[i]->second){  //もしランキング内より頻度高ければ挿入発生
                 
	for(int j = freq-1; j > i; j--){ //2位のものは3位へ、3位のものは4位へずれていく。。
	  number_of_word[j] = number_of_word[j-1];	  

	}
	number_of_word[i] = p;//ずらし終えたら挿入
	break;//挿入したら次の単語へ。breakしないと複数回挿入してしまう。
      }
    }
  }
}

//-------------------------------------------------------
void OutputWordRank(int num_of_rank, StrIntMap::iterator word_freq_rank[],int total){
  for (int i = 0;i < num_of_rank; i++){       //word_freq_rankの中身を標準出力
    string f = word_freq_rank[i]->first;
    int    s = word_freq_rank[i]->second;
    double percentage =  s/ (double)total;
 
    cout << f.append(15 - f.length(),' ')  << " occurred "
	 <<  setw(5) <<  s << " times."
	 << "     "  <<  setprecision(5) <<  percentage << " %" <<endl;
  }
  cout << "total = " << total << "words" << endl;

  /*
    for (StrIntMap::iterator p = w.begin();  //単語名でソートされた全単語を頻度とともに出力
    p != w.end() ; ++p){
    string s = p->first;
    cout << s.append(20 - s.length(),' ')  << " occurred "
    << p->second << " times.\n";
  */
}
//---------------------------------------------------------

void CountNgram(istream& f, ngram_Count& count ,int* total, int n){
  cout << "counting n-grams(n = " << n << ")..........."; 
  string s;
  int i = 0;
  vector<string> words;
 
  while(f >> s){
    s = Lowercase (s);    
    s = Trim(s);         
    words.push_back(s);
    ++i;
  }

  ngram_Pair ngram;

 
  for (int j = 0; j != i-1; j++){
    string following_word = "";
    for(int k = 1; k != n; ++k)
      if(j+k < i){
	{
	  following_word = following_word + words[j+k];
	  following_word = following_word + " ";
	}
      }
    ngram.first = words[j];
    ngram.second = following_word;
    ++count[ngram];
    ++*total;
  }
  cout << "done\n";

}



//-------------ngramでの挿入ソート---------------------------------
void NgramSort(ngram_Count& ngram, ngram_Count::iterator number_of_word[], int freq){
  cout << "sorting n-grams...........";
  ngram_Count::iterator p = ngram.begin();//wの先頭freq個の単語をngram_Countに格納
  
  for(int i = 0 ; i<freq ; i++){    //初期化がわり。   
    number_of_word[i] = p;
    ++p;
  }
  
  for (ngram_Count::iterator p = ngram.begin(); p != ngram.end(); ++p){
    int s = p->second;                     //sはpのさす単語の頻度。
	
    for(int i = 0; i < freq; i++){        //頻度ランキングを順番に。
      if(s > number_of_word[i]->second){  //もしランキング内より頻度高ければ挿入発生
                   
	for(int j = freq-1; j > i; j--){ //2位のものは3位へ、3位のものは4位へずれていく。。
	  number_of_word[j] = number_of_word[j-1];	  

	}
	number_of_word[i] = p;//ずらし終えたら挿入
	break;//挿入したら次の単語へ。breakしないと複数回挿入してしまう。
      }
    }
  }
  cout << "done\n";
}

//---------------頻度順のngramの配列をngram_freq_rank個まで表示---------------
void OutputNgramRank(int num_of_rank, ngram_Count::iterator ngram_freq_rank[],int total){
  
  for (int i = 0;i < num_of_rank; i++){       //word_freq_rankの中身を標準出力
    string key = ngram_freq_rank[i]->first.first;
    string following_word = ngram_freq_rank[i]->first.second;
    int    s = ngram_freq_rank[i]->second;
    double percentage =  s / (double)total;
    string sequence = key + " ";
    sequence += following_word;

    cout << sequence.append(20 - sequence.length(),' ')   
      // << following_word.append(15 - following_word.length(),' ')
	 <<  setw(5) <<  s << "/" <<total << " times."
	 << "     "  <<  setprecision(5) <<  percentage << " %" <<endl;
  }
  cout << "total = " << total << "ngrams" << endl;
  

}
//-------------ある単語(key)の次に何がくるか頻度順に格納------------
void Search_by_Key(string key, ngram_Count& ngram, ngram_Count::iterator number_of_word[], int freq){
  cout << "sorting n-grams...........";
  ngram_Count::iterator p = ngram.begin();//wの先頭freq個の単語をngram_Countに格納
 
  for(int i = 0 ; i<freq; p++ ){    //初期化がわり。 
    if (p->first.first == key){  
      number_of_word[i] = p;
      i++;
    }   
  }
  
  for (ngram_Count::iterator p = ngram.begin(); p != ngram.end(); ++p){
    if (p->first.first == key){  
      int s = p->second;                     //sはpのさす単語の頻度。
	

      for(int i = 0; i < freq; i++){        //頻度ランキングを順番に。
	if(s > number_of_word[i]->second){  //もしランキング内より頻度高ければ挿入発生
                  
	  for(int j = freq-1; j > i; j--){ //2位のものは3位へ、3位のものは4位へずれていく。。
	    number_of_word[j] = number_of_word[j-1];	  

	  }
	  number_of_word[i] = p;//ずらし終えたら挿入
	  break;//挿入したら次の単語へ。breakしないと複数回挿入してしまう。
	}
      }
    }
  }
  cout << "done\n";
}




//---------------------------main--------------------------
int main() {

  string temp;
  int num_of_rank = 50;//頻度上位何単語持ってくるか
  int total = 0; //全単語数
  int ngram = 3;
  string key = "i";

  cout << "input filename.\n";
  cin >> temp;
  ifstream f(temp.c_str());    //.c_str()つけるのがルールみたい
 

  clock_t  start, end;
  start = clock();

  /*
  //------単語頻度を調べるときはこっち
  StrIntMap::iterator word_freq_rank[num_of_rank];//上位単語のイテレータを入れる配列
  StrIntMap w;//単語と・その頻度を入れる配列
  CountWords(f, w, &total);//fの単語をその頻度とともにmap wに格納
  OutputWordRank(num_of_rank, word_freq_rank, total);
  */
  //------ngramを調べる時はこっち
  ngram_Count::iterator ngram_freq_rank[num_of_rank];
  ngram_Count count;
  CountNgram(f, count, &total, ngram);
  //Search_by_Key(key,count,ngram_freq_rank, num_of_rank);//keyの次に続く語を調べる。keyを指定しないときはNgramSortを使用
  NgramSort(count,ngram_freq_rank, num_of_rank);//wの中から上位num_of_rank個をnumber_of_wordに格納
  cout << endl;
  OutputNgramRank(num_of_rank, ngram_freq_rank, total);

  end = clock();
  cout << (end-start)/CLOCKS_PER_SEC <<" sec" <<endl;
 
 


    }





