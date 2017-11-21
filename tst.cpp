
#include "HLS/hls.h"
#include <stdio.h>
#include <stdlib.h>

#define COUNT 1000

typedef ihc::stream_in<  float, ihc::usesPackets<true> > ast_pkt_snk;
typedef ihc::stream_out< float, ihc::usesPackets<true> > ast_pkt_src;

typedef struct {
  float sub;
  float mult;
} amm_csr;

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

  float res = 0;
  float expect = 0;
  printf("Check\n");
  for( int i = 0; i < COUNT; i++){
    res = c.read();
    expect = ( a_tmp[i] * b_tmp[i] - arg.sub ) * arg.mult;

    if( expect != res ){
      printf( "Error[%d]: ", i );
      printf( "(%e * %e - %e) * %e = %e\n", a_tmp[i], b_tmp[i], arg.sub, arg.mult, res );
    }
  }
  printf( "End\n" );
  return 0;  
}
