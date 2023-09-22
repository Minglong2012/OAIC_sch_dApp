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
 * From ASN.1 module "E2SM-gNB-NRT-IEs"
 * 	found in "../asnTextFiles/e2sm-gNB-NRT-v401.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -fno-include-deps -findirect-choice -gen-PER -no-gen-OER -D.`
 */

#ifndef	_NRT_TableRecord_H_
#define	_NRT_TableRecord_H_


#include "asn_application.h"

/* Including external dependencies */
#include "OCTET_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NRT-TableRecord */
typedef OCTET_STRING_t	 NRT_TableRecord_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NRT_TableRecord;
asn_struct_free_f NRT_TableRecord_free;
asn_struct_print_f NRT_TableRecord_print;
asn_constr_check_f NRT_TableRecord_constraint;
ber_type_decoder_f NRT_TableRecord_decode_ber;
der_type_encoder_f NRT_TableRecord_encode_der;
xer_type_decoder_f NRT_TableRecord_decode_xer;
xer_type_encoder_f NRT_TableRecord_encode_xer;
per_type_decoder_f NRT_TableRecord_decode_uper;
per_type_encoder_f NRT_TableRecord_encode_uper;
per_type_decoder_f NRT_TableRecord_decode_aper;
per_type_encoder_f NRT_TableRecord_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _NRT_TableRecord_H_ */
#include "asn_internal.h"
