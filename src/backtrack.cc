/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include <map>
using namespace std;


Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

vector<int> matching_order;
int sum = 0 ;
int NumOfQueryVertics = 0;

//For matching_order
bool compare(const pair<int,int>& a, const pair<int,int>& b) {
	if (a.second == b.second) return a.first < b.first;
	return a.second < b.second;
}

vector<Vertex> MatchingOrder(const Graph &query, const CandidateSet &cs){
  map<int, int> m;
  for (int i=0; i < NumOfQueryVertics; i++){
    m[i]=cs.GetCandidateSize(i);
  }
  vector<pair<int,int>> vec( m.begin(), m.end() );
	sort(vec.begin(), vec.end(), compare);

  vector<int> result;
  for (auto num : vec) {
		result.push_back(num.first);
    std::cout<<num.first<<' ';
	}

  return result;
}

void search(const Graph &data, const Graph &query, const CandidateSet &cs, size_t now_index, map<int,int> result_map){

  if (now_index == NumOfQueryVertics - 1){
    //print all
    ++sum;
    for(map<int,int>::iterator it=result_map.begin(); it!=result_map.end(); ++it){
      std::cout<< it->second << " ";
    }
    std::cout<< "\n";
    if (sum==100000){
      exit(0);
    }
  }else{

    //select next_query_id
    size_t next_query_id = matching_order[now_index+1];

    //inspect next candidate
    
    size_t candidate_size = cs.GetCandidateSize(next_query_id);
  
    for (int count = 0 ; count < candidate_size; ++count){
      size_t candidate = cs.GetCandidate(next_query_id,count);
      bool vaild = true ;
      //check candidate is used.
      for (map<int,int>::iterator it = result_map.begin(); it != result_map.end(); ++it){
        if (it->second == candidate){
          vaild = false;
          break;
        }else{
          //do nothing
        }
      }
      if (vaild == true){
        //check candidate is connected
        //check all neighbor of query_id.
        for (int i = query.GetNeighborStartOffset(next_query_id); i <query.GetNeighborEndOffset(next_query_id) ; i++){
          //check neighbor is in result_map
          auto pos = result_map.find(query.GetNeighbor(i));
          if ( pos == result_map.end() ) {
            // not found, then do nothing
          } else {
            // found, check they are conndected.
            if (data.IsNeighbor(candidate, pos->second)){
              //do nothing
            } else{
            vaild = false;
            break;
            }
          }
        }
        if (vaild == true){
          //this candidate is vaild.
          result_map[next_query_id] = candidate;
          search(data, query, cs, now_index+1, result_map);
        }else{
          //do nothing
        }
      }else{
        //do nothing
      }

    }
  }
}
       

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  ios::sync_with_stdio(false);

  map<int, int> result_map; //list for printout
  int now_index = 0 ; //set start
  NumOfQueryVertics = query.GetNumVertices();
  matching_order = MatchingOrder(query, cs);

  for (int count = 0 ; count < cs.GetCandidateSize(now_index); ++count){
    result_map.clear();
    size_t candidate = cs.GetCandidate(now_index,count);
    result_map[now_index]=candidate;
    search(data, query, cs, now_index, result_map);
  }
}