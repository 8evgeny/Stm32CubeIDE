/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : mbedtls.c
  * Description        : This file provides code for the configuration
  *                      of the mbedtls instances.
  ******************************************************************************
  ******************************************************************************
   * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "mbedtls.h"

/* USER CODE BEGIN 0 */
static const uint8_t *pers = (uint8_t *)("ssl_server");
#define mbedtls_printf UART_Printf
#include "main.h"
#include "mbedtls/certs.h"
void UART_Printf(const char* fmt, ...);
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Global variables ---------------------------------------------------------*/
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;
mbedtls_x509_crt cert;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_entropy_context entropy;

/* USER CODE BEGIN 2 */
mbedtls_pk_context pkey;
/* USER CODE END 2 */

/* MBEDTLS init function */
void MX_MBEDTLS_Init(void)
{
   /**
  */
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_x509_crt_init(&cert);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_entropy_init( &entropy );
  /* USER CODE BEGIN 3 */
  int ret, len;
  mbedtls_pk_init( &pkey );
  /*
   * 1. Load the certificates and private RSA key
   */
  UART_Printf( "\n  . Loading the server cert. and key..." );
  /*
   * This demonstration program uses embedded test certificates.
   * Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
   * server and CA certificates, as well as mbedtls_pk_parse_keyfile().
   */
  ret = mbedtls_x509_crt_parse( &cert, (const unsigned char *)
                                mbedtls_test_srv_crt_ec_pem, mbedtls_test_srv_crt_ec_pem_len );
  if( ret != 0 )
  {
    UART_Printf( " failed !!!  mbedtls_x509_crt_parse returned_1 %d\r\n", ret );
//    goto exit;
  }

  ret = mbedtls_x509_crt_parse( &cert, (const unsigned char *)
                                mbedtls_test_cas_pem, mbedtls_test_cas_pem_len );
  if( ret != 0 )
  {
    UART_Printf( " failed !!!  mbedtls_x509_crt_parse returned_2 %d\r\n", ret );
//    goto exit;
  }

  ret =  mbedtls_pk_parse_key( &pkey, (const unsigned char *) mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0 );
  if( ret != 0 )
  {
    UART_Printf( " failed\n  !  mbedtls_pk_parse_key returned_3 %d\r\n", ret );
//    goto exit;
  }

  UART_Printf( " ok\n" );

//  /*
//   * 2. Setup the listening TCP socket
//   */
//  mbedtls_printf( "  . Bind on https://localhost:4433/ ..." );

//  if((ret = mbedtls_net_bind(&listen_fd, NULL, "4433", MBEDTLS_NET_PROTO_TCP )) != 0)
//  {
//    mbedtls_printf( " failed\n  ! mbedtls_net_bind returned %d\n\n", ret );
//    goto exit;
//  }

//  mbedtls_printf( " ok\n" );

//  /*
//   * 3. Seed the RNG
//   */
//  mbedtls_printf( "  . Seeding the random number generator..." );
//  ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
//                                     (const unsigned char *) pers, strlen( (char *)pers));
//  if( ret != 0 )
//  {
//    mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
////    goto exit;
//  }
//  mbedtls_printf( " ok\n" );

  /*
    * 4. Setup stuff
    */
   mbedtls_printf( "  . Setting up the SSL data...." );

   if( ( ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
   {
     mbedtls_printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
//     goto exit;
   }

   mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

 #if defined(MBEDTLS_SSL_CACHE_C)
   mbedtls_ssl_conf_session_cache(&conf, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
 #endif

   mbedtls_ssl_conf_ca_chain(&conf, cert.next, NULL);
   if( ( ret = mbedtls_ssl_conf_own_cert(&conf, &cert, &pkey ) ) != 0)
   {
     mbedtls_printf( " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret );
//     goto exit;
   }

   if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
   {
     mbedtls_printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
//     goto exit;
   }
   mbedtls_printf( " ok\n" );

  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @}
  */

/**
  * @}
  */

