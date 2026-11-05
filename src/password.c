#include "password.h"

bool password_generate(
    char *output,
    size_t output_capacity,
    const password_profile_t *profile,
    random_u32_fn random_u32,
    void *random_context
){
		
	// Create a dictionary for the new password based
	// on the profile preferences
	char password_dict[128];
	size_t dict_size = 0;
	
  if(profile->lowercase){
		for(size_t i = 0; lowercase[i] != '\0'; i++)
			password_dict[dict_size++] = lowercase[i];
	}
	if(profile->uppercase){
		for(size_t i = 0; uppercase[i] != '\0'; i++)
			password_dict[dict_size++] = uppercase[i];
	}
	if(profile->digits){
		for(size_t i = 0; digits[i] != '\0'; i++)
			password_dict[dict_size++] = digits[i];
	}
	if(profile->specials == SPECIAL_ALL){
		for(size_t i = 0; all_specials[i] != '\0'; i++)
			password_dict[dict_size++] = all_specials[i];
	}
	else if(profile->specials == SPECIAL_SAFE){
		for(size_t i = 0; safe_specials[i] != '\0'; i++)
			password_dict[dict_size++] = safe_specials[i];
	}
	
	if (dict_size == 0) {
    return false;
}
	
	// Generating random num and creating password
	for (size_t i = 0; i < profile->length; i++){
		uint32_t random_number = random_u32(random_context);
		uint32_t index = random_number % dict_size;
		output[i] = password_dict[index];
	}
	output[profile->length] = '\0';
	return true;
}