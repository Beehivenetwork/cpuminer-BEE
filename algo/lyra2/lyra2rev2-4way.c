#include "lyra2-gate.h"
#include <memory.h>

#if defined (LYRA2REV2_4WAY)	

#include "algo/blake/blake-hash-4way.h"
#include "algo/keccak/keccak-hash-4way.h"
#include "algo/skein/skein-hash-4way.h"
#include "algo/bmw/bmw-hash-4way.h"
#include "algo/cubehash/cubehash_sse2.h" 

typedef struct {
   blake256_4way_context     blake;
   keccak256_4way_context    keccak;
   cubehashParam             cube;
   skein256_4way_context     skein;
   bmw256_4way_context          bmw;
} lyra2v2_4way_ctx_holder;

static lyra2v2_4way_ctx_holder l2v2_4way_ctx;

bool init_lyra2rev2_4way_ctx()
{
   keccak256_4way_init( &l2v2_4way_ctx.keccak );
   cubehashInit( &l2v2_4way_ctx.cube, 256, 16, 32 );
   skein256_4way_init( &l2v2_4way_ctx.skein );
   bmw256_4way_init( &l2v2_4way_ctx.bmw );
   return true;
}

void lyra2rev2_4way_hash( void *state, const void *input )
{
   uint32_t hash0[8] __attribute__ ((aligned (64)));
   uint32_t hash1[8] __attribute__ ((aligned (32)));
   uint32_t hash2[8] __attribute__ ((aligned (32)));
   uint32_t hash3[8] __attribute__ ((aligned (32)));
   uint32_t vhash[8*4] __attribute__ ((aligned (64)));
   uint64_t vhash64[4*4] __attribute__ ((aligned (64)));
   lyra2v2_4way_ctx_holder ctx __attribute__ ((aligned (64))); 
   memcpy( &ctx, &l2v2_4way_ctx, sizeof(l2v2_4way_ctx) );

   blake256_4way( &ctx.blake, input + (64<<2), 16 );
   blake256_4way_close( &ctx.blake, vhash );

   mm256_rintrlv_4x32_4x64( vhash64, vhash, 256 );

   keccak256_4way( &ctx.keccak, vhash64, 32 );
   keccak256_4way_close( &ctx.keccak, vhash64 );

   mm256_dintrlv_4x64( hash0, hash1, hash2, hash3, vhash64, 256 );

   cubehashUpdateDigest( &ctx.cube, (byte*) hash0, (const byte*) hash0, 32 );
   cubehashInit( &ctx.cube, 256, 16, 32 );
   cubehashUpdateDigest( &ctx.cube, (byte*) hash1, (const byte*) hash1, 32 );
   cubehashInit( &ctx.cube, 256, 16, 32 );
   cubehashUpdateDigest( &ctx.cube, (byte*) hash2, (const byte*) hash2, 32 );
   cubehashInit( &ctx.cube, 256, 16, 32 );
   cubehashUpdateDigest( &ctx.cube, (byte*) hash3, (const byte*) hash3, 32 );

   LYRA2REV2( l2v2_wholeMatrix, hash0, 32, hash0, 32, hash0, 32, 1, 4, 4 );
   LYRA2REV2( l2v2_wholeMatrix, hash1, 32, hash1, 32, hash1, 32, 1, 4, 4 );
   LYRA2REV2( l2v2_wholeMatrix, hash2, 32, hash2, 32, hash2, 32, 1, 4, 4 );
   LYRA2REV2( l2v2_wholeMatrix, hash3, 32, hash3, 32, hash3, 32, 1, 4, 4 );

   mm256_intrlv_4x64( vhash64, hash0, hash1, hash2, hash3, 256 );

   skein256_4way( &ctx.skein, vhash64, 32 );
   skein256_4way_close( &ctx.skein, vhash64 );

   mm256_dintrlv_4x64( hash0, hash1, hash2, hash3, vhash64, 256 );

   cubehashInit( &ctx.cube, 256, 16, 32 );
   cubehashUpdateDigest( &ctx.cube, (byte*) hash0, (const byte*) hash0, 32 );
   cubehashInit( &ctx.cube, 256, 16, 32 );
   cubehashUpdateDigest( &ctx.cube, (byte*) hash1, (const byte*) hash1, 32 );
   cubehashInit( &ctx.cube, 256, 16, 32 );
   cubehashUpdateDigest( &ctx.cube, (byte*) hash2, (const byte*) hash2, 32 );
   cubehashInit( &ctx.cube, 256, 16, 32 );
   cubehashUpdateDigest( &ctx.cube, (byte*) hash3, (const byte*) hash3, 32 );

   intrlv_4x32( vhash, hash0, hash1, hash2, hash3, 256 );

   bmw256_4way( &ctx.bmw, vhash, 32 );
   bmw256_4way_close( &ctx.bmw, state );
}

int scanhash_lyra2rev2_4way( struct work *work, uint32_t max_nonce,
                             uint64_t *hashes_done, struct thr_info *mythr )
{
   uint32_t hash[8*4] __attribute__ ((aligned (64)));
   uint32_t vdata[20*4] __attribute__ ((aligned (64)));
   uint32_t *hash7 = &(hash[7<<2]);
   uint32_t lane_hash[8] __attribute__ ((aligned (32)));
   uint32_t *pdata = work->data;
   uint32_t *ptarget = work->target;
   const uint32_t first_nonce = pdata[19];
   uint32_t n = first_nonce;
   const uint32_t Htarg = ptarget[7];
   __m128i *noncev = (__m128i*)vdata + 19;   // aligned
   int thr_id = mythr->id;  // thr_id arg is deprecated

   if ( opt_benchmark )
      ( (uint32_t*)ptarget )[7] = 0x0000ff;

   mm128_bswap_intrlv80_4x32( vdata, pdata );

   blake256_4way_init( &l2v2_4way_ctx.blake );
   blake256_4way( &l2v2_4way_ctx.blake, vdata, 64 );

   do
   {
      *noncev = mm128_bswap_32( _mm_set_epi32( n+3, n+2, n+1, n ) );

      lyra2rev2_4way_hash( hash, vdata );
      pdata[19] = n;

      for ( int lane = 0; lane < 4; lane++ ) if ( hash7[lane] <= Htarg )
      {
         extr_lane_4x32( lane_hash, hash, lane, 256 );
         if ( fulltest( lane_hash, ptarget ) && !opt_benchmark )
         {
            pdata[19] = n + lane;         
            submit_lane_solution( work, lane_hash, mythr, lane );
         }
      }
      n += 4;
   } while ( (n < max_nonce-4) && !work_restart[thr_id].restart);
   *hashes_done = n - first_nonce + 1;
   return 0;
}

#endif
