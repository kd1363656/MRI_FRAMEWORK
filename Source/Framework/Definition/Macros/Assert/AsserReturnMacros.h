#pragma once

#define FWK_ASSERT_RETURN(a_conditions, a_message) \
if (a_conditions)								   \
{												   \
	assert(false && message);					   \
	return;										   \
}												   \