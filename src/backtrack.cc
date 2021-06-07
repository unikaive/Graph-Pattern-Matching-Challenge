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

//For query ordering
bool compare(const pair<int,int>& a, const pair<int,int>& b) {
	if (a.second == b.second) return a.first < b.first;
	return a.second < b.second;
}

vector<Vertex> query_ordering(const Graph &query, const CandidateSet &cs){
  map<int, int> m;
  for (int i=0; i < query.GetNumVertices(); i++){
    m[i]=cs.GetCandidateSize(i);
  }
  vector<pair<int,int>> vec( m.begin(), m.end() );
	sort(vec.begin(), vec.end(), compare);

  vector<int> result;
  // for (auto num : vec) {
	// 	cout << "key: "<< num.first << " | value: " << num.second << "\n";
	// }
  for (auto num : vec) {
		result.push_back(num.first);
	}

  return result;
}

void search(const Graph &data, const Graph &query, const CandidateSet &cs, size_t now_index, map<int,int> result_map){
  int NumOfQueryVertics = query.GetNumVertices();

  if (now_index == NumOfQueryVertics - 1){
    //print all
    sum+=1;
    std::map<int,int>::iterator it;
    for(it=result_map.begin(); it!=result_map.end(); ++it){
      std::cout<< it->second << " ";
    }
    std::cout<< "\n";
  }else{

    //select next_query_id
    size_t next_query_id = matching_order[now_index+1];

    //inspect next candidate
    for (int count2 = 0 ; count2 < cs.GetCandidateSize(next_query_id); ++count2){
      size_t candidate = cs.GetCandidate(next_query_id,count2);
      bool vaild = true ;

      //check candidate is used.
      for (auto it = result_map.begin(); it != result_map.end(); ++it)
        if (it->second == candidate){
          vaild = false;
          break;
        }else{
          //do nothing
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

  // for (int i = 0; i < query.GetNumVertices(); i++)
  // {
  //   matching_order.push_back(i);
  // }
  matching_order = query_ordering(query, cs);

  int now_index = 0 ;

  //list for printout
  std::map<int, int> result_map;
  
  for (int count = 0 ; count < cs.GetCandidateSize(now_index); ++count){
    result_map.clear();
    size_t candidate = cs.GetCandidate(now_index,count);
    result_map[now_index]=candidate;
    search(data, query, cs, now_index, result_map);
  }
  std::cout<< sum << " ";


  // int NumOfQueryVertics = query.GetNumVertices();

  // std::cout << "t " << NumOfQueryVertics << "\n";

  // visited.assign(NumOfQueryVertics, false); // if visited = True,
  // int start = 0;
  // index = start;
  // s.push(index);
  // cout<< index << "";

  // query.GetNeighbor(index);

  // implement your code here.
  // for (int count = 0 ; count < NumOfQueryVertics; ++count)
  //   {
  //     int temp = cs.GetCandidateSize(count);
  //     std::cout << temp << "\n";
  //     for (int count2 = 0 ; count2 < temp; ++count2)
  //     {
  //     int temp2 = cs.GetCandidate(count,count2);
  //     std::cout << temp2 << " ";
  //     }
  //   }


  //for test
  // size_t test_vertex = 10 ;
  // size_t test_label = 78;
  // std::cout << "GetGraphID : " << query.GetGraphID() << "\n";
  // std::cout << "GetNumVertices() :" << query.GetNumVertices() << "\n";
  // std::cout << "GetNumEdges() : " << query.GetNumEdges()<< "\n";
  // std::cout << "GetNumLabels() : " << query.GetNumLabels() << "\n";
  // std::cout << "GetLabelFrequency : " << query.GetLabelFrequency(test_label)<< "\n";
  // std::cout << "GetDegree : " << query.GetDegree(test_vertex)<< "\n";
  // std::cout << "GetNeighborStartOffset(Vertex v) : " << query.GetNeighborStartOffset(test_vertex) << "\n";
  // std::cout << "GetNeighborEndOffset(Vertex v) : " << query.GetNeighborEndOffset(test_vertex) << "\n";
  // std::cout << "GetNeighborStartOffset(Vertex v, Label l)" << query.GetNeighborStartOffset(test_vertex, test_label) << "\n";
  // std::cout << "GetNeighborEndOffset(Vertex v, Label l)" << query.GetNeighborEndOffset(test_vertex, test_label) << "\n";
  // std::cout << "GetLabel(Vertex v) : " << query.GetLabel(test_vertex) << "\n";
  // std::cout << "GetNeighbor(size_t offset) : " << query.GetNeighbor(50) << "\n";
  // std::cout << "IsNeighbor(Vertex u, Vertex v) :" << query.IsNeighbor(10, 11) << "\n";
  // for (int i=query.GetNeighborStartOffset(test_vertex); i <query.GetNeighborEndOffset(test_vertex) ; i++){
  //   std::cout << query.GetNeighbor(i)<< "\n";
  // }
}

// void dfs(int start, std::vector<int> graph[], bool check[]){
// 	check[start]= true;
// 	printf("%d ", start);

// 	for(int i=0; i < graph[start].size(); i++){
// 		int next = graph[start][i];
// 		// 방문하지 않았다면
// 		if(check[next]==false){
// 			// 재귀함수를 호출한다.
// 			dfs(next, graph, check);
// 		}
// 	}
// }
// void DFS(int i)
// 	{
// 		vector<int>::const_iterator next;

// 		visited[i] = true;
// 		printf("%d\n",i);

// 		for(next = adj[i].begin() ; next != adj[i].end() ; next++) 
// 		{
// 			if(visited[*next] != true){				
// 				DFS(*next);
// 			}
// 		}

// 	}

 
//     }
//     //check vailidate of candidates
//     for check next Candidate
//     if vaildate{
//       add to result_list
//       do recursive
//     }else{
//       //do nothing, end.
//     }
//   }
//   ---
  

  

//   //Check Neighbor Vertices of current vertex
//   for (int i=query.GetNeighborStartOffset(now_vertex); i <query.GetNeighborEndOffset(now_vertex) ; i++){
//     if (visited[query.GetNeighbor(i)]){
//       if (i==query.GetNeighborEndOffset(now_vertex)-1){

//       }else{
//         //Do nothing
//       }
//     }else{
//       search(data, query, cs, now_vertex, visited, result_list);
//     }
    
//   }

//   if (all naibor viertixes is visited){
//     if (check visted falsed){
//       search that id
//     }
//     else{ //if all visted is true, print result
//       for (int count = 0; count < query.GetNumVertices();++count){
//         std::cout << result_list[count] << " ";
//         }
//       std::cout <<  "\n";
//     }
//   }else{
//     search naibor vertices
    
//   }
//   //if done, print result_list
//   if (now_vertex == query.GetNumVertices()){
//     for (int count = 0; count < query.GetNumVertices();++count){
//       std::cout << result_list[count] << " ";
//       }
//     std::cout <<  "\n";
//     sum = sum+1;

//   }else{
//   // else put element to result_list, and do recursive.
//     for (int count2 = 0 ; count2 < cs.GetCandidateSize(now_vertex); ++count2)
//     {
//     int temp2 = cs.GetCandidate(now_vertex,count2);
//     result_list[now_vertex] = temp2;
//     search(data, query, cs, now_vertex+1, visited, result_list);
//     }
//   }
// }