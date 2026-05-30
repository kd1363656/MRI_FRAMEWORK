#pragma once

#define FWK_ASSERT_RETURN(a_message) \
assert(false && a_message);			 \
return;		 

#define FWK_ASSERT_RETURN_VALUE(a_message, a_returnValue) \
assert(false && a_message);							      \
return a_returnValue;								      

#define FWK_ASSERT_RETURN_IF(a_conditions, a_message) \
if (a_conditions)								      \
{												      \
	assert(false && a_message);					      \
	return;										      \
}

#define FWK_ASSERT_RETURN_VALUE_IF(a_conditions, a_message, a_returnValue) \
if (a_conditions)														   \
{																		   \
	assert(false && a_message);											   \
	return a_returnValue;												   \
}