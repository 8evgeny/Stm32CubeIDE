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
  UART_Printf( "\n  . Loading the server cert. and key...\r\n" );
  /*
   * This demonstration program uses embedded test certificates.
   * Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
   * server and CA certificates, as well as mbedtls_pk_parse_keyfile().
   */
  ret = mbedtls_x509_crt_parse( &cert, (const unsigned char *)
                                mbedtls_test_srv_crt_ec_pem, mbedtls_test_srv_crt_ec_pem_len );
  if( ret != 0 )
  {
    UART_Printf( " failed !!!  mbedtls_x509_crt_parse returned %d\r\n", ret );
//    goto exit;
  }


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

