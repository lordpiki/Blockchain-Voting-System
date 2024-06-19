#pragma once

//#define BYTE char

enum Codes
{
	PUBLISH_VOTE = (char) 10,
	PUBLISH_BLOCK = (char) 20,
	IBD = (char) 30,
	IBD_RESPONSE = (char) 31,
	GET_SIGNED_PK_REQUEST = (char) 60,
	GET_SIGNED_PK_RESPONSE = (char) 61
};