// RUN: %clang_cc1 -fsyntax-only -verify -std=c++11 %s

struct notlit {
  notlit() {}
};
struct notlit2 {
  notlit2() {}
};

// valid declarations
constexpr int i1 = 0;
constexpr int f1() { return 0; }
struct s1 {
  constexpr static int mi1 = 0;
  const static int mi2;
};
constexpr int s1::mi2 = 0;

// invalid declarations
// not a definition of an object
constexpr extern int i2; // expected-error {{constexpr variable declaration must be a definition}}
// not a literal type
constexpr notlit nl1; // expected-error {{constexpr variable 'nl1' must be initialized by a constant expression}} expected-note {{non-literal type 'const notlit' cannot be used in a constant expression}}
// function parameters
void f2(constexpr int i) {} // expected-error {{function parameter cannot be constexpr}}
// non-static member
struct s2 {
  constexpr int mi1; // expected-error {{non-static data member cannot be constexpr}}
  static constexpr int mi2; // expected-error {{requires an initializer}}
};
// typedef
typedef constexpr int CI; // expected-error {{typedef cannot be constexpr}}
// tag
constexpr class C1 {}; // expected-error {{class cannot be marked constexpr}}
constexpr struct S1 {}; // expected-error {{struct cannot be marked constexpr}}
constexpr union U1 {}; // expected-error {{union cannot be marked constexpr}}
constexpr enum E1 {}; // expected-error {{enum cannot be marked constexpr}}
template <typename T> constexpr class TC1 {}; // expected-error {{class cannot be marked constexpr}}
template <typename T> constexpr struct TS1 {}; // expected-error {{struct cannot be marked constexpr}}
template <typename T> constexpr union TU1 {}; // expected-error {{union cannot be marked constexpr}}
class C2 {} constexpr; // expected-error {{class cannot be marked constexpr}}
struct S2 {} constexpr; // expected-error {{struct cannot be marked constexpr}}
union U2 {} constexpr; // expected-error {{union cannot be marked constexpr}}
enum E2 {} constexpr; // expected-error {{enum cannot be marked constexpr}}
// FIXME: Mark default constructors as 'constexpr' when appropriate.
constexpr class C3 {} c3 = C3(); // unexpected-error {{must be initialized by a constant expression}} unexpected-note {{non-constexpr constructor}} unexpected-note {{here}}
constexpr struct S3 {} s3 = S3(); // unexpected-error {{must be initialized by a constant expression}} unexpected-note {{non-constexpr constructor}} unexpected-note {{here}}
constexpr union U3 {} u3 = {};
constexpr enum E3 { V3 } e3 = V3;
class C4 {} constexpr c4 = C4(); // unexpected-error {{must be initialized by a constant expression}} unexpected-note {{non-constexpr constructor}} unexpected-note {{here}}
struct S4 {} constexpr s4 = S4(); // unexpected-error {{must be initialized by a constant expression}} unexpected-note {{non-constexpr constructor}} unexpected-note {{here}}
union U4 {} constexpr u4 = {};
enum E4 { V4 } constexpr e4 = V4;
constexpr int; // expected-error {{constexpr can only be used in variable and function declarations}}
// redeclaration mismatch
constexpr int f3(); // expected-note {{previous declaration is here}}
int f3(); // expected-error {{non-constexpr declaration of 'f3' follows constexpr declaration}}
int f4(); // expected-note {{previous declaration is here}}
constexpr int f4(); // expected-error {{constexpr declaration of 'f4' follows non-constexpr declaration}}
template<typename T> constexpr T f5(T);
template<typename T> constexpr T f5(T); // expected-note {{previous}}
template<typename T> T f5(T); // expected-error {{non-constexpr declaration of 'f5' follows constexpr declaration}}
template<typename T> T f6(T); // expected-note {{here}}
template<typename T> constexpr T f6(T); // expected-error {{constexpr declaration of 'f6' follows non-constexpr declaration}}
// destructor
struct ConstexprDtor {
  constexpr ~ConstexprDtor() = default; // expected-error {{destructor cannot be marked constexpr}}
};

// template stuff
template <typename T> constexpr T ft(T t) { return t; }
template <typename T> T gt(T t) { return t; }
struct S {
  template<typename T> constexpr T f();
  template<typename T> T g() const;
};

// explicit specialization can differ in constepxr
// FIXME: When checking the explicit specialization, we implicitly instantiate
// the primary template then claim a constexpr mismatch.
template <> notlit ft(notlit nl) { return nl; }
template <> char ft(char c) { return c; } // desired-note {{previous}} unexpected-error {{follows constexpr declaration}} unexpected-note {{here}}
template <> constexpr char ft(char nl); // desired-error {{constexpr declaration of 'ft<char>' follows non-constexpr declaration}}
template <> constexpr int gt(int nl) { return nl; } // unexpected-error {{follows non-constexpr declaration}} unexpected-note {{here}}
template <> notlit S::f() const { return notlit(); }
template <> constexpr int S::g() { return 0; } // desired-note {{previous}} unexpected-error {{follows non-constexpr declaration}} unexpected-note {{here}}
template <> int S::g() const; // desired-error {{non-constexpr declaration of 'g<int>' follows constexpr declaration}}
// specializations can drop the 'constexpr' but not the implied 'const'.
template <> char S::g() { return 0; } // expected-error {{no function template matches}}
template <> double S::g() const { return 0; } // ok

constexpr int i3 = ft(1);

void test() {
  // ignore constexpr when instantiating with non-literal
  notlit2 nl2;
  (void)ft(nl2);
}

// Examples from the standard:
constexpr int square(int x); // expected-note {{declared here}}
constexpr int bufsz = 1024;

constexpr struct pixel { // expected-error {{struct cannot be marked constexpr}}
  int x;
  int y;
  constexpr pixel(int);
};

constexpr pixel::pixel(int a)
  : x(square(a)), y(square(a)) // expected-note {{undefined function 'square' cannot be used in a constant expression}}
  { }

constexpr pixel small(2); // expected-error {{must be initialized by a constant expression}} expected-note {{in call to 'pixel(2)'}}

constexpr int square(int x) {
  return x * x;
}

constexpr pixel large(4);

int next(constexpr int x) { // expected-error {{function parameter cannot be constexpr}}
      return x + 1;
}

extern constexpr int memsz; // expected-error {{constexpr variable declaration must be a definition}}
