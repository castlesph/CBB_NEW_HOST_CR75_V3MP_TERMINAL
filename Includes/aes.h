#ifndef __AES_H__
#define __AES_H__

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
    uint32 erk[64];     // encryption round keys 
    uint32 drk[64];     // decryption round keys
    int nr;             // number of rounds 
}
aes_context;

//--------------------------------------------------------------------------
// AES_Init
//	use AES_Init once only at the begining of the program to initialize AES functions below.
//--------------------------------------------------------------------------
// Parameters
//	NULL
//--------------------------------------------------------------------------
void AES_Init(void);

//--------------------------------------------------------------------------
// AESDecrypt
//	use AES way to decrypt data
//--------------------------------------------------------------------------
// Parameters
//	key :  input AES key
//	keylen : input bits (128 OR 192 OR 256)
//	input : input data to be decrypt
//	output : output aes decrypt data
//--------------------------------------------------------------------------
void AESDecrypt(uint8 *key, uint32 keylen,uint8 *input, uint8 *output);

//--------------------------------------------------------------------------
// AESEncrypt
//	use AES way to encrypt data
//--------------------------------------------------------------------------
// Parameters
//	key :  input AES key
//	keylen : input bits (128 OR 192 OR 256)
//	input : input data to be encrypt
//	output : output aes encrypt data
//--------------------------------------------------------------------------
void AESEncrypt(uint8 *key, uint32 keylen, uint8 *input, uint8 *output);


//--------------------------------------------------------------------------
// AESDecryptCBC
//	use AES CBC Mode to decrypt data 
//--------------------------------------------------------------------------
// Parameters
//	key :  input AES key
//	iv : Init Vector (16 BYTES)
//	length : input data length
//	input : input data to be decrypt
//	length : output decrypt use aes cbc mode data 
//--------------------------------------------------------------------------
void AESDecryptCBC(uint8 *key,uint8 *iv,uint32 length,uint8 *input,uint8 *output);

//--------------------------------------------------------------------------
// AESEncryptCBC
//	use AES CBC Mode to encrypt data 
//--------------------------------------------------------------------------
// Parameters
//	key :  input AES key
//	iv : Init Vector (16 BYTES)
//	length : input data length
//	input : input data to be encrypt
//	length : output encrypt use aes cbc mode data 
//--------------------------------------------------------------------------
void AESEncryptCBC(uint8 *key,uint8 *iv,uint32 length,uint8 *input,uint8 *output);

//--------------------------------------------------------------------------
// AES_CMAC
//	This function will calculate the AES-CMAC
//--------------------------------------------------------------------------
// Parameters
//	key :  input AES key
//	piv : Init Vector (16 bytes)
//	length : input data length
//	input : input data to calculate AES CMAC
//	mac : output CMAC (16 bytes)
//--------------------------------------------------------------------------
void AES_CMAC(uint8 *key, uint8 *piv,uint32 length,uint8 *input, uint8 *mac);

//--------------------------------------------------------------------------
// AES_CMAC
//	This function can make the AES SubKey K1 and K2
//--------------------------------------------------------------------------
// Parameters
//	key :  input AES key
//	inlen : input data length
//	iv : Init Vector (16 BYTES)
//	K1 : output Key1
//	K2 : output Key2
//--------------------------------------------------------------------------
void AESMAC_GenerateSubKey(uint8 *key,uint8 inlen, uint8* iv,uint8 *K1, uint8 *K2);

//--------------------------------------------------------------------------
// AES_CMAC
//	This function will do AES padding data
//--------------------------------------------------------------------------
// Parameters
//	lastb :  input last block value( enough 16 bytes data )
//	pad : output padding data (16 bytes)
//	length : input enough 16 bytes data length
//--------------------------------------------------------------------------
void AESMAC_Padding(uint8 *lastb, uint8 *pad,uint32 length);

//--------------------------------------------------------------------------
// AES_CMAC
//	This function will do XOR with 128 bits 
//--------------------------------------------------------------------------
// Parameters
//	a :  input a data (16 bytes)
//	b :  input b data (16 bytes)
//	out : output data for a xor b 
//--------------------------------------------------------------------------
void AESMAC_Xor128(uint8 *a, uint8 *b, uint8 *out);

#endif
