// RUN: %clang_cc1 -triple i686-pc-win32 -cxx-abi microsoft -fms-extensions -verify %s

typedef void void_fun_t();
typedef void __cdecl cdecl_fun_t();

// Pointers to free functions
void            free_func_default(); // expected-note 2 {{previous declaration is here}}
void __cdecl    free_func_cdecl(); // expected-note 2 {{previous declaration is here}}
void __stdcall  free_func_stdcall(); // expected-note 2 {{previous declaration is here}}
void __fastcall free_func_fastcall(); // expected-note 2 {{previous declaration is here}}

void __cdecl    free_func_default();
void __stdcall  free_func_default(); // expected-error {{function declared 'stdcall' here was previously declared without calling convention}}
void __fastcall free_func_default(); // expected-error {{function declared 'fastcall' here was previously declared without calling convention}}

void            free_func_cdecl();
void __stdcall  free_func_cdecl(); // expected-error {{function declared 'stdcall' here was previously declared 'cdecl'}}
void __fastcall free_func_cdecl(); // expected-error {{function declared 'fastcall' here was previously declared 'cdecl'}}

void            free_func_stdcall();
void __cdecl    free_func_stdcall(); // expected-error {{function declared 'cdecl' here was previously declared 'stdcall'}}
void __fastcall free_func_stdcall(); // expected-error {{function declared 'fastcall' here was previously declared 'stdcall'}}

void __cdecl    free_func_fastcall(); // expected-error {{function declared 'cdecl' here was previously declared 'fastcall'}}
void __stdcall  free_func_fastcall(); // expected-error {{function declared 'stdcall' here was previously declared 'fastcall'}}
void            free_func_fastcall();

// Overloaded functions may have different calling conventions
void __fastcall free_func_default(int);
void __cdecl    free_func_default(int *);

void __thiscall free_func_cdecl(char *);
void __cdecl    free_func_cdecl(double);

typedef void void_fun_t();
typedef void __cdecl cdecl_fun_t();

// Pointers to member functions
struct S {
  void            member_default1(); // expected-note {{previous declaration is here}}
  void            member_default2();
  void __cdecl    member_cdecl1();
  void __cdecl    member_cdecl2(); // expected-note {{previous declaration is here}}
  void __thiscall member_thiscall1();
  void __thiscall member_thiscall2(); // expected-note {{previous declaration is here}}

  // Typedefs carrying the __cdecl convention are adjusted to __thiscall.
  void_fun_t           member_typedef_default; // expected-note {{previous declaration is here}}
  cdecl_fun_t          member_typedef_cdecl1;  // expected-note {{previous declaration is here}}
  cdecl_fun_t __cdecl  member_typedef_cdecl2;
  void_fun_t __stdcall member_typedef_stdcall;

  // Static member functions can't be __thiscall
  static void            static_member_default1();
  static void            static_member_default2();
  static void            static_member_default3(); // expected-note {{previous declaration is here}}
  static void __cdecl    static_member_cdecl1();
  static void __cdecl    static_member_cdecl2(); // expected-note {{previous declaration is here}}
  static void __stdcall  static_member_stdcall1();
  static void __stdcall  static_member_stdcall2();

  // Variadic functions can't be other than default or __cdecl
  void            member_variadic_default(int x, ...);
  void __cdecl    member_variadic_cdecl(int x, ...);

  static void            static_member_variadic_default(int x, ...);
  static void __cdecl    static_member_variadic_cdecl(int x, ...);
};

void __cdecl    S::member_default1() {} // expected-error {{function declared 'cdecl' here was previously declared without calling convention}}
void __thiscall S::member_default2() {}

void __cdecl   S::member_typedef_default() {} // expected-error {{function declared 'cdecl' here was previously declared without calling convention}}
void __cdecl   S::member_typedef_cdecl1() {} // expected-error {{function declared 'cdecl' here was previously declared without calling convention}}
void __cdecl   S::member_typedef_cdecl2() {}
void __stdcall S::member_typedef_stdcall() {}

void            S::member_cdecl1() {}
void __thiscall S::member_cdecl2() {} // expected-error {{function declared 'thiscall' here was previously declared 'cdecl'}}

void            S::member_thiscall1() {}
void __cdecl    S::member_thiscall2() {} // expected-error {{function declared 'cdecl' here was previously declared 'thiscall'}}

void            S::static_member_default1() {}
void __cdecl    S::static_member_default2() {}
void __stdcall  S::static_member_default3() {} // expected-error {{function declared 'stdcall' here was previously declared without calling convention}}

void            S::static_member_cdecl1() {}
void __stdcall  S::static_member_cdecl2() {} // expected-error {{function declared 'stdcall' here was previously declared 'cdecl'}}

void __cdecl    S::member_variadic_default(int x, ...) { (void)x; }
void            S::member_variadic_cdecl(int x, ...) { (void)x; }

void __cdecl    S::static_member_variadic_default(int x, ...) { (void)x; }
void            S::static_member_variadic_cdecl(int x, ...) { (void)x; }

// Declare a template using a calling convention.
template <class CharT> inline int __cdecl mystrlen(const CharT *str) {
  int i;
  for (i = 0; str[i]; i++) { }
  return i;
}
extern int sse_strlen(const char *str);
template <> inline int __cdecl mystrlen(const char *str) {
  return sse_strlen(str);
}
void use_tmpl(const char *str, const int *ints) {
  mystrlen(str);
  mystrlen(ints);
}

struct MixedCCStaticOverload {
  static void overloaded(int a);
  static void __stdcall overloaded(short a);
};

void MixedCCStaticOverload::overloaded(int a) {}
void MixedCCStaticOverload::overloaded(short a) {}

// Friend function decls are cdecl by default, not thiscall.  Friend method
// decls should always be redeclarations, because the class cannot be
// incomplete.
struct FriendClass {
  void friend_method() {}
};
void __stdcall friend_stdcall1() {}
class MakeFriendDecls {
  int x;
  friend void FriendClass::friend_method();
  friend void              friend_default();
  friend void              friend_stdcall1();
  friend void __stdcall    friend_stdcall2();
  friend void              friend_stdcall3(); // expected-note {{previous declaration is here}}
};
void           friend_default() {}
void __stdcall friend_stdcall3() {} // expected-error {{function declared 'stdcall' here was previously declared without calling convention}}
void __stdcall friend_stdcall2() {}

// Test functions with multiple attributes.
void __attribute__((noreturn)) __stdcall __attribute__((regparm(1))) multi_attribute(int x);
void multi_attribute(int x) { __builtin_unreachable(); }


// expected-error@+2 {{stdcall and cdecl attributes are not compatible}}
// expected-error@+1 {{fastcall and cdecl attributes are not compatible}}
void __cdecl __cdecl __stdcall __cdecl __fastcall multi_cc(int x);
