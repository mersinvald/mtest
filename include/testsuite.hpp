#ifndef TESTSUITE_H
#define TESTSUITE_H
#ifndef __CC_ARM
#include <iostream>
#include <string>
#endif
#include <stdint.h>
#include <string.h>

struct Report {
    uint32_t overall;
    uint32_t passed;
};

#ifndef __CC_ARM
#define TESTCASE_PARAMS std::string& name
#else
#define TESTCASE_PARAMS char* name
#endif

class TestSuite {
protected:
    template<typename T>
    static bool compare(const T var, const T ans) {
        if(var != ans) {
            #ifndef __CC_ARM
            std::cout << "expected: " << ans << std::endl;
            std::cout << "got:      " << var << std::endl;
            #endif
            return false;
        }
        return true;
    }

    static bool compare(const char* var, const char* ans, bool compare_len) {
        size_t varlen = strlen(var);
        size_t anslen = strlen(ans);

        if(varlen != anslen && compare_len) {
            #ifndef __CC_ARM
            std::cout << "expected length " << anslen << ", "
                      << "got " << varlen << std::endl;
            #endif
            return false;
        }

        size_t cnt = (compare_len) ? strlen(var) : strlen(ans);

        if(strncmp(var, ans, cnt) != 0) {
            #ifndef __CC_ARM
            std::cout << "expected: " << ans << std::endl;
            std::cout << "got:      " << var << std::endl;
            #endif
            return false;
        }

        return true;
    }

    static bool compare(const void *vvar, const void *vans, size_t varlen, size_t anslen = 0) {
        char* var = (char*) vvar;
        char* ans = (char*) vans;
        if(!anslen) anslen = varlen;
        if(varlen != anslen) {
            #ifndef __CC_ARM
            std::cout << "expected length " << anslen << ", "
                      << "got " << varlen << std::endl;
            #endif
            return false;
        }

        if(memcmp(var, ans, varlen) != 0) {
            #ifndef __CC_ARM
            std::cout << "expected: "; print_array(ans, anslen);
            std::cout << "got:      "; print_array(var, varlen);
            #endif
            return false;
        }

        return true;
    }

    static bool test(bool (*fptr)(TESTCASE_PARAMS)) {
#ifndef __CC_ARM
        std::string name;
        bool ok = fptr(name);
        std::cout << name << ": " << (ok ? "SUCCESS" : "FAILURE") << std::endl;
#else
        char name[128];
        bool ok = fptr(name);
#endif
        return ok;
    }

    // Find substring
    static bool has_substr(void* buffer, size_t bufsize, void* substr, size_t substrsize) {
    #if defined __GNUG__ && !defined __CC_ARM // If GNU extensions are present, use memmem
        return (memmem(buffer, bufsize, substr, substrsize) != NULL);
    #elif !defined __CC_ARM           // Else do dirty trick with strings
        std::string haystick((char*)buffer, (char*)buffer+bufsize);
        std::string needle((char*)substr, (char*)substr+substrsize);
        return haystick.find(needle) != std::string::npos;
    #else
        char* needle = (char*) substr;
        char* haystick = (char*) buffer;
        while((haystick = (char*) memchr(haystick, needle[0], bufsize)) != NULL) {
            if(memcmp(haystick, needle, substrsize) == 0) return true;
        }
        return false;
    #endif
    }

    #ifndef __CC_ARM
    #define INIT_TESTCASE               \
            name = __FUNCTION__;  \
            int  _stnum = 0;              \
            bool _ok = true;               \
            bool _temp;


    #define SUBTEST(expr) \
            _stnum++;      \
            _temp = (expr); \
            _ok &= _temp;    \
            if(!_temp) {      \
                std::cout << __FUNCTION__ << ":"                      \
                          << "SubTest #" << _stnum << " \"" #expr "\"" \
                          << " failed.\n"; \
            }
    #else
    #define INIT_TESTCASE               \
        strcpy(name, __FUNCTION__);  \
        int  _stnum = 0;              \
        bool _ok = true;               \
        bool _temp;

    #define SUBTEST(expr) \
            _stnum++;      \
            _temp = (expr); \
            _ok &= _temp;    \
            if(!_temp) {      \
                RETURN;        \
            }
    #endif

    #define RETURN \
            return _ok;

private:
    #ifndef __CC_ARM
    static void print_array(char *array, size_t length) {
        std::cout << std::hex;
        for(size_t i = 0; i < length; i++) {
            std::cout << (int)array[i] << " ";
        }
        std::cout << std::endl;
    }
    #endif
};

#endif // TESTSUITE_H
