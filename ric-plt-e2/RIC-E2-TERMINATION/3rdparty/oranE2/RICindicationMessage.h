/*
 *
 * Copyright 2020 AT&T Intellectual Property
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "e2ap-v01.01.00.asn"
 * 	`asn1c -pdu=auto -fincludes-quoted -fcompound-names -findirect-choice -fno-include-deps -gen-PER -no-gen-OER -no-gen-example`
 */

#ifndef	_RICindicationMessage_H_
#define	_RICindicationMessage_H_


#include "asn_application.h"

/* Including external dependencies */
#include "OCTET_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RICindicationMessage */
typedef OCTET_STRING_t	 RICindicationMessage_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RICindicationMessage;
asn_struct_free_f RICindicationMessage_free;
asn_struct_print_f RICindicationMessage_print;
asn_constr_check_f RICindicationMessage_constraint;
ber_type_decoder_f RICindicationMessage_decode_ber;
der_type_encoder_f RICindicationMessage_encode_der;
xer_type_decoder_f RICindicationMessage_decode_xer;
xer_type_encoder_f RICindicationMessage_encode_xer;
per_type_decoder_f RICindicationMessage_decode_uper;
per_type_encoder_f RICindicationMessage_encode_uper;
per_type_decoder_f RICindicationMessage_decode_aper;
per_type_encoder_f RICindicationMessage_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _RICindicationMessage_H_ */
#include "asn_internal.h"
