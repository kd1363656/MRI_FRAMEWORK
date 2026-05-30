#pragma once

#define FWK_ASSERT_RETURN(a_conditions, a_message) \
if (a_conditions)								   \
{												   \
	assert(false && a_message);					   \
	return;										   \
}

#define FWK_ASSERT_RETURN_WITH_VALUE(a_conditions, a_message, a_returnValue) \
if (a_conditions)														     \
{																		     \
	assert(false && a_message);											     \
	return a_returnValue;												     \
}