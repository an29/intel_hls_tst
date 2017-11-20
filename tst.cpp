
#include "HLS/hls.h"
#include <stdio.h>
#include <stdlib.h>

#define COUNT 30

typedef ihc::stream_in<  float, ihc::usesPackets<true> > ast_pkt_snk;
typedef ihc::stream_out< float, ihc::usesPackets<true> > ast_pkt_src;

typedef struct amm_csr {
  float sub;
  float mult;
};

hls_avalon_slave_component
component
void mult( ast_pkt_snk& a, 
           ast_pkt_snk& b, 
           ast_pkt_src& c,
           hls_avalon_slave_register_argument amm_csr arg
          ) {
  bool sop = false;
  bool eop = false; 
  float tmp = 0;
  while( !eop ){
    tmp = ( a.read( sop, eop ) * b.read() - arg.sub ) * arg.mult;
    c.write( tmp, sop, eop );
  }
}


int main(void){

  printf( "Start\n" );
  ast_pkt_snk a;
  ast_pkt_snk b; 
  ast_pkt_src c;

  srand(3);
  float a_tmp[COUNT];
  float b_tmp[COUNT];
  amm_csr  arg = {2.0,3.0};
  for( int i = 0; i < COUNT; i++){
    a_tmp[i] = rand();
    b_tmp[i] = rand();
    a.write( a_tmp[i], i == 0, i == (COUNT -1 ) );
    b.write( b_tmp[i], i == 0, i == (COUNT -1 ) );
  }

  mult( a, b, c, arg );

  printf("Check:\n");
  for( int i = 0; i < COUNT; i++){
    printf( "(%e * %e - %e) * %e = %e\n", a_tmp[i], b_tmp[i], arg.sub, arg.mult, c.read() );
  }
  return 0;  
}
