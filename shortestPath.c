#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "jrb.h"
#include "dllist.h"

#define max 100
#define INFINITIVE_VALUE 10000000

typedef struct 
{
  JRB edges;
  JRB vertices;
}Graph;

typedef char array[max];

typedef struct 
{
  char nameline[5]; //ten tuyen
  int line[max];  //ds cac diem dung trong tuyen
  int countStation; //so diem dung trong tuyen
}infoline;

infoline oneline[max];

char file[]="busStations.txt";


array nameBusStop[max];  //ten cac diem dung
int noStation=-1,noLine=-1; //noStation: so diem dung, noLine: so tuyen

void input(){
  FILE *f;
  char s[max],new[max];
  int i;
  f = fopen(file,"r");
  if (f == NULL)
  {
    printf("Không mở được file\n");
    exit(0);
  }
  else{
    while(!feof(f)){
      fgets(s,105,f);
      if (s[0]=='S')
      {
        noStation++;
        for (i = 0; i < strlen(s); ++i)
        {
          if (s[i]=='=')
          {
            strncpy(nameBusStop[noStation],s+i+1,strlen(s)-i-3);
            break;
          }
        }
      }

      if (s[0]=='L')
      {
        noLine++;
        for (i = 0; i < strlen(s); ++i)
        {
          if (s[i]=='=')
          {
            strncpy(oneline[noLine].nameline,s,i);
            break;
          }
        }
        pushInfoLine(s,noLine);
      }
    }
    // noStation = noStation - 1;
    noLine = noLine - 1;
  }
  fclose(f);
}

void pushInfoLine(char s[max],int x){
  char num[max];
  int i=0,j=0,len=strlen(s),count=0;
  while (i<len && j<len){
    if (s[i]!='S') i++;
    if (s[j]!=' ') j++; 
    if (s[i]=='S' && s[j]==' '){
      strncpy(num,s+i+1,j-i);
      oneline[x].line[count] = atoi(num);
      count++;
      i++;
      j++;
    }
  }
  oneline[x].countStation = count;
}

Graph createGraph();
void addVertex(Graph graph, int id, char* name);
char *getVertex(Graph graph, int id);
char *getNameLine(int v1, int v2);

void addEdge(Graph graph, int v1, int v2, double weight);
double getEdgeValue(Graph graph, int v1, int v2);

int indegree(Graph graph, int v, int* output);
int outdegree(Graph graph, int v, int* output);

double shortestPath(Graph graph, int s, int t, int *path, int *length);
void dropGraph(Graph graph);

int main(){
  input();
  int i, j, status, src,des;
  double distance_s_t;
  int s,t,path[100],length, choice;
  char startPlace[max],desPlace[max];
  Graph g = createGraph();
  for (i = 0; i <= noStation; ++i)
  {
    addVertex(g,i,nameBusStop[i]);
  }

  for (i = 0; i <= noLine; ++i)
  {
    for (j = 0; j < oneline[i].countStation - 1; ++j)
    {
      src = oneline[i].line[j];
      des = oneline[i].line[j+1];
      addEdge(g,src,des,1);
    }
  }

  do{
    printf("------------------------------\n");
    printf("1.Xem danh sách các điểm dừng\n");
    printf("2.Xem danh sách các tuyến bus\n");
    printf("3.Tìm tuyến bus theo điểm dừng\n");
    printf("4.Thoát\n");
    printf("Lựa chọn: ");scanf("%d",&choice);
    printf("\n");
    switch(choice)
    {
      case 1: printf("DANH SÁCH CÁC ĐIỂM DỪNG\n\n");
              for (i = 0; i <= noStation; ++i)
              {
                printf("%d %s\n",i+1,nameBusStop[i]);
              }
              printf("\n");
              break;

      case 2:
              printf("DANH SÁCH CÁC TUYẾN BUS\n\n");
              for (i = 0; i <= noLine; ++i)
              {
                printf("%s: \n",oneline[i].nameline);
                for (j = 0; j < oneline[i].countStation; ++j)
                {
                  status = oneline[i].line[j];
                  if (j < oneline[i].countStation-1)
                  {
                    printf("%s -> ",nameBusStop[status]);
                  }
                  else printf("%s\n",nameBusStop[status]);
                }
                printf("\n");
              }
              // printf("\n");
              break;

      case 3:
              // printf("Enter start place: ");scanf("%[^\n]%*c",startPlace);
              printf("ĐIỂM XUẤT PHÁT: ");scanf("%s",startPlace);
              for (i = 0; i <= noStation; ++i)
              {
                if (strcmp(nameBusStop[i],startPlace)==0)
                {
                  s = i;
                  break;
                }
              }
              // printf("Enter destination place: ");scanf("%[^\n]%*c",desPlace);
              printf("ĐIỂM ĐẾN: ");scanf("%s",desPlace);
              for (i = 0; i <= noStation; ++i)
              {
                if (strcmp(nameBusStop[i],desPlace)==0)
                {
                  t = i;
                  break;
                }
              }

              printf("\n");

              distance_s_t = shortestPath(g,s,t,path,&length);
              if (distance_s_t != INFINITIVE_VALUE){
                printf("ĐƯỜNG ĐI: \n");
                for (i=length-1; i>=0; i--){
                  printf("%d ", path[i]);
                  status = path[i];
                  if (i>0)
                  {
                    printf("%s -> ",nameBusStop[status]);
                  }
                  else printf("%s\n",nameBusStop[status]);
                  // printf("S%d %s\n",status,nameBusStop[status]);
                }
                printf("\n");
                printf("CÁC TUYẾN BUS CẦN ĐI: \n");
                for (i=length-1; i>0; i--){
                  printf("%s -> %s: %s\n",nameBusStop[path[i]],nameBusStop[path[i-1]],getNameLine(path[i],path[i-1]));
                }
              }else{
                printf("kHÔNG TÌM ĐƯỢC TUYẾN BUS TỪ %s -> %s\n",startPlace,desPlace);
              }
              printf("\n");
              break;
    }
  }while(choice!=4);
}



char *getNameLine(int v1, int v2){
  int status = 0;
  for (int i = 0; i <= noLine; ++i)
  {
    for (int j = 0; j < oneline[i].countStation-1; ++j)
    {
      if (oneline[i].line[j]==v1 && oneline[i].line[j+1]==v2){
        return oneline[i].nameline;
        status = 1;
        break;
      }
    }
    if (status == 1) break;
  }
}


Graph createGraph(){
  Graph g;
  g.edges = make_jrb();
  g.vertices = make_jrb();
  return g;
}

void dropGraph(Graph graph){
  jrb_free_tree(graph.edges);
  jrb_free_tree(graph.vertices);
}

void addVertex(Graph graph, int id, char* name){
  JRB node = jrb_find_int(graph.vertices, id);
  if (node==NULL) // only add new vertex
      jrb_insert_int(graph.vertices, id, new_jval_s(strdup(name)));
}

char *getVertex(Graph graph, int id){
  JRB node = jrb_find_int(graph.vertices, id);
  if (node==NULL)
      return NULL;
  else
      return jval_s(node->val);
}

// Make connection between v1 and v2
void addEdge(Graph graph, int v1, int v2,double weight){
  JRB node, tree;
  if (getEdgeValue(graph, v1, v2) == INFINITIVE_VALUE)
  {
      node = jrb_find_int(graph.edges, v1);
      if (node == NULL)
      {
          tree = make_jrb();
          jrb_insert_int(graph.edges, v1, new_jval_v(tree));
      }
      else
      {
          tree = (JRB)jval_v(node->val);
      }
      jrb_insert_int(tree, v2, new_jval_d(weight));
  }
}

// Get weight from v1 to v2
double getEdgeValue(Graph graph, int v1, int v2){
  JRB node = jrb_find_int(graph.edges, v1);
  if (node == NULL)
  {
      return INFINITIVE_VALUE;
  }
  JRB tree = (JRB)jval_v(node->val);
  JRB node1 = jrb_find_int(tree, v2);
  if (node1 == NULL) {
      return INFINITIVE_VALUE;
  }
  return jval_d(node1->val);
}

// Liet ke nhung dinh {w} ma ton tai canh w-->v
int indegree(Graph graph, int v, int* output){
  JRB tree, node;
  int total = 0;
  jrb_traverse(node, graph.edges){
      tree = (JRB) jval_v(node->val);
      if (jrb_find_int(tree, v))
      {
          output[total] = jval_i(node->key);
          total++;
      }
  }
  return total;
}

// Liet ke nhung dinh {w} ma ton tai canh v-->w
int outdegree(Graph graph, int v, int* output){
  JRB tree, node;
  int total;
  node = jrb_find_int(graph.edges, v);
  if (node==NULL)
      return 0;
  tree = (JRB) jval_v(node->val);
  total = 0;
  jrb_traverse(node, tree){
      output[total] = jval_i(node->key);
      total++;
  }
  return total;
}

double shortestPath(Graph graph, int s, int t, int *path, int *length){
  // Khoi tao cac distance = 0
  double distance[1000], min;
  int previous[1000], u, visit[1000];
  
  for (int i=0; i<1000; i++){
      distance[i] = INFINITIVE_VALUE;
      visit[i] = 0;
      previous[i] = 0;
  }
  distance[s] = 0;
  previous[s] = s;
  visit[s] = 1;
  
  Dllist ptr, queue, node;
  queue = new_dllist();
  dll_append(queue, new_jval_i(s));
  
  // Duyet Queue
  while (!dll_empty(queue)){
      min = INFINITIVE_VALUE;
      dll_traverse(ptr, queue){
          // Lay ra min{distance}
          u = jval_i(ptr->val);
          if (min > distance[u]){
              min = distance[u];
              node = ptr;
          }
      }
      u = jval_i(node->val);
      visit[u] = 1;
      dll_delete_node(node);
      if (u == t) break;
      
      int output[100];
      int n = outdegree(graph, u, output);
      
      // Cap nhap distance cua tat ca cac dinh ma lien ke voi dinh min
      for (int i=0; i<n; i++){
          int v = output[i];
          double w = getEdgeValue(graph, u, v);
          if (distance[v] > distance[u] + w){
              distance[v] = distance[u] + w;
              previous[v] = u;
          }
          if (visit[v] == 0){
              dll_append(queue, new_jval_i(v));
          }
      }
  }
      
  // Truy vet lai de lay duong di tuw s ---> t va luu trong path[]
  double distance_s_t = distance[t];
  int count = 0;
  if (distance[t] != INFINITIVE_VALUE){
      path[count++] = t;
      while (t != s){
          t = previous[t];
          path[count++] = t;
      }
      *length = count;
  }
  return distance_s_t;
}