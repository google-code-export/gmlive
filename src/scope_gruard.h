/*
 * =====================================================================================
 * 
 *       Filename:  scope_gruard.h
 * 
 *    Description: 资源自动释放，用于异常安全。
 * 
 *        Version:  1.0
 *        Created:  2007年08月21日 11时00分50秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  wind (xihe), xihels@gmail.com
 *        Company:  cyclone
 * 
 * =====================================================================================
 */
#ifndef _SCOPE_GRUARD_H_
#define _SCOPE_GRUARD_H_

class ScopeGuardImplBase {
	public:
		void dismiss() const throw ()
		{ dismissed = true; }

	protected:
		ScopeGuardImplBase() : dismissed (false) {}
		ScopeGuardImplBase(const ScopeGuardImplBase& other)
			: dismissed (other.dismissed)
		{ other.dismiss(); }

		~ScopeGuardImplBase() {} // nonvirtual.

		mutable bool dismissed;
	private:
		// Disable assignment.
		ScopeGuardImplBase& operator = (const ScopeGuardImplBase&);
};

template <typename Fun>
class ScopeGuardImpl0 : public ScopeGuardImplBase {
	public:
		ScopeGuardImpl0(const Fun& _fun)
			: fun(_fun) {}	
		~ScopeGuardImpl0()
		{
			try {
				if (!dismissed) fun();
			} catch (...) {}
		}
	private:
		Fun fun;
};

template <typename Fun>
	ScopeGuardImpl0<Fun>
MakeGuard(const Fun& fun)
{
	return ScopeGuardImpl0<Fun>(fun);
}

template <typename Fun, typename Parm>
class ScopeGuardImpl1 : public ScopeGuardImplBase {
	public:
		ScopeGuardImpl1(const Fun& _fun, const Parm& _parm)
			: fun(_fun), parm(_parm) {}

		~ScopeGuardImpl1()
		{
			try {
				if (!dismissed) fun(parm);
			} catch (...) {}
		}

	private:
		Fun fun;
		const Parm parm;
};

template <typename Fun, typename Parm>
	ScopeGuardImpl1<Fun, Parm>
MakeGuard(const Fun& fun, const Parm& parm)
{
	return ScopeGuardImpl1<Fun, Parm>(fun, parm);
}

template <typename Fun, typename Parm1, typename Parm2>
class ScopeGuardImpl2 : public ScopeGuardImplBase {
	public:
		ScopeGuardImpl2(const Fun& _fun, 
				const Parm1& _parm1, 
				const Parm2& _parm2)
			: fun(_fun),
			parm1(_parm1),
			parm2(_parm2) {}

		~ScopeGuardImpl2()
		{
			try {
				if (!dismissed) fun(parm1, parm2);
			} catch (...) {}
		}
	private:
		Fun fun;
		const Parm1 parm1;
		const Parm2 parm2; 
};

template <typename Fun, typename Parm1, typename Parm2>
	ScopeGuardImpl2<Fun, Parm1, Parm2>
MakeGuard(const Fun& fun, const Parm1& parm1, const Parm2& parm2)
{
	return ScopeGuardImpl2<Fun, Parm1, Parm2>(fun, parm1, parm2);
}

template <typename Fun, typename Parm1, typename Parm2, typename Parm3>
class ScopeGuardImpl3 : public ScopeGuardImplBase {
	public:
		ScopeGuardImpl3(const Fun& _fun, 
				const Parm1& _parm1, 
				const Parm2& _parm2,
				const Parm3& _parm3)
			: fun(_fun),
			parm1(_parm1),
			parm2(_parm2),
			parm3(_parm3) {}

		~ScopeGuardImpl3()
		{
			try {
				if (!dismissed) fun(parm1, parm2, parm3);
			} catch (...) {}
		}
	private:
		Fun fun;
		const Parm1 parm1;
		const Parm2 parm2; 
		const Parm3 parm3; 
};

template <typename Fun, typename Parm1, typename Parm2, typename Parm3>
	ScopeGuardImpl3<Fun, Parm1, Parm2, Parm3>
MakeGuard(const Fun& fun,
		const Parm1& parm1,
		const Parm2& parm2, 
		const Parm3& parm3)
{
	return ScopeGuardImpl3<Fun, Parm1, Parm2, Parm3>(fun, parm1, parm2, parm3);
}


template <typename Fun, typename Parm1, typename Parm2,
	 typename Parm3, typename Parm4>
	 class ScopeGuardImpl4 : public ScopeGuardImplBase {
		 public:
			 ScopeGuardImpl4(const Fun& _fun, 
					 const Parm1& _parm1, 
					 const Parm2& _parm2,
					 const Parm3& _parm3,
					 const Parm4& _parm4)
				 : fun(_fun),
				 parm1(_parm1),
				 parm2(_parm2),
				 parm3(_parm3),
				 parm4(_parm4) {}

			 ~ScopeGuardImpl4()
			 {
				 try {
					 if (!dismissed) fun(parm1, parm2, parm3, parm4);
				 } catch (...) {}
			 }
		 private:
			 Fun fun;
			 const Parm1 parm1;
			 const Parm2 parm2; 
			 const Parm3 parm3; 
			 const Parm4 parm4;
	 };

template <typename Fun, typename Parm1, typename Parm2,
	 typename Parm3, typename Parm4>
	ScopeGuardImpl4<Fun, Parm1, Parm2, Parm3, Parm4>
MakeGuard(const Fun& fun,
		const Parm1& parm1,
		const Parm2& parm2, 
		const Parm3& parm3,
		const Parm4& parm4)
{
	return ScopeGuardImpl4<Fun, Parm1, Parm2, Parm3, Parm4>
		(fun, parm1, parm2, parm3, parm4);
}




template <typename Obj, typename Fun>
class ObjScopeGuardImpl0 : public ScopeGuardImplBase {
	public:
		ObjScopeGuardImpl0(Obj& _obj, const Fun& _fun)
			: obj(_obj), 
			fun(_fun) {}	
		~ObjScopeGuardImpl0()
		{
			try {
				if (!dismissed) (obj.*fun)();
			} catch (...) {}
		}
	private:
		Obj& obj;
		Fun fun;
};

template <typename Obj, typename Fun>
	ObjScopeGuardImpl0<Obj, Fun>
MakeGuard(Obj& obj, const Fun& fun)
{
	return ObjScopeGuardImpl0<Obj, Fun>(obj, fun);
}

template <typename Obj, typename Fun, typename Parm>
class ObjScopeGuardImpl1 : public ScopeGuardImplBase {
	public:
		ObjScopeGuardImpl1(Obj& _obj, const Fun& _fun, const Parm& _parm)
			: obj(_obj), fun(_fun), parm(_parm) {}

		~ObjScopeGuardImpl1()
		{
			try {
				if (!dismissed) (obj.*fun)(parm);
			} catch (...) {}
		}

	private:
		Obj& obj;
		Fun fun;
		const Parm parm;
};

template <typename Obj, typename Fun, typename Parm>
	ObjScopeGuardImpl1<Obj, Fun, Parm>
MakeGuard(Obj& obj, const Fun& fun, const Parm& parm)
{
	return ObjScopeGuardImpl1<Obj, Fun, Parm>(obj, fun, parm);
}

template <typename Obj, typename Fun, typename Parm1, typename Parm2>
class ObjScopeGuardImpl2 : public ScopeGuardImplBase {
	public:
		ObjScopeGuardImpl2(Obj& _obj,
				const Fun& _fun, 
				const Parm1& _parm1, 
				const Parm2& _parm2)
			: obj(_obj),
			fun(_fun),
			parm1(_parm1),
			parm2(_parm2) {}

		~ObjScopeGuardImpl2()
		{
			try {
				if (!dismissed) (obj.*fun)(parm1, parm2);
			} catch (...) {}
		}
	private:
		Obj& obj;
		Fun fun;
		const Parm1 parm1;
		const Parm2 parm2; 
};

template <typename Obj, typename Fun, typename Parm1, typename Parm2>
	ObjScopeGuardImpl2<Obj, Fun, Parm1, Parm2>
MakeGuard(Obj& obj, const Fun& fun, const Parm1& parm1, const Parm2& parm2)
{
	return ObjScopeGuardImpl2<Obj, Fun, Parm1, Parm2>
		(obj, fun, parm1, parm2);
}

template <typename Obj, typename Fun, typename Parm1, typename Parm2, typename Parm3>
class ObjScopeGuardImpl3 : public ScopeGuardImplBase {
	public:
		ObjScopeGuardImpl3(Obj& _obj, 
				const Fun& _fun, 
				const Parm1& _parm1, 
				const Parm2& _parm2,
				const Parm3& _parm3)
			: obj(_obj),
			fun(_fun),
			parm1(_parm1),
			parm2(_parm2),
			parm3(_parm3) {}

		~ObjScopeGuardImpl3()
		{
			try {
				if (!dismissed) (obj.*fun)(parm1, parm2, parm3);
			} catch (...) {}
		}
	private:
		Obj& obj;
		Fun fun;
		const Parm1 parm1;
		const Parm2 parm2; 
		const Parm3 parm3; 
};

template <typename Obj, typename Fun, typename Parm1, typename Parm2, typename Parm3>
	ObjScopeGuardImpl3<Obj, Fun, Parm1, Parm2, Parm3>
MakeGuard(Obj& obj,
		const Fun& fun,
		const Parm1& parm1,
		const Parm2& parm2, 
		const Parm3& parm3)
{
	return ObjScopeGuardImpl3<Obj, Fun, Parm1, Parm2, Parm3>
		(obj, fun, parm1, parm2, parm3);
}


template <typename Obj, typename Fun, typename Parm1, typename Parm2,
	 typename Parm3, typename Parm4>
	 class ObjScopeGuardImpl4 : public ScopeGuardImplBase {
		 public:
			 ObjScopeGuardImpl4(Obj& _obj,
					 const Fun& _fun, 
					 const Parm1& _parm1, 
					 const Parm2& _parm2,
					 const Parm3& _parm3,
					 const Parm4& _parm4)
				 :obj(_obj),
				 fun(_fun),
				 parm1(_parm1),
				 parm2(_parm2),
				 parm3(_parm3),
				 parm4(_parm4) {}

			 ~ObjScopeGuardImpl4()
			 {
				 try {
					 if (!dismissed) (obj.*fun)(parm1, parm2, parm3, parm4);
				 } catch (...) {}
			 }
		 private:
			 Obj& obj;
			 Fun fun;
			 const Parm1 parm1;
			 const Parm2 parm2; 
			 const Parm3 parm3; 
			 const Parm4 parm4;
	 };

template <typename Obj, typename Fun, typename Parm1, typename Parm2,
	 typename Parm3, typename Parm4>
	ObjScopeGuardImpl4<Obj, Fun, Parm1, Parm2, Parm3, Parm4>
MakeGuard(Obj& obj, 
		const Fun& fun,
		const Parm1& parm1,
		const Parm2& parm2, 
		const Parm3& parm3,
		const Parm4& parm4)
{
	return ObjScopeGuardImpl4<Obj, Fun, Parm1, Parm2, Parm3, Parm4>
		(obj, fun, parm1, parm2, parm3, parm4);
}

// 使用一个辅助类，可以把引用转变为一个值，
// 以适应一些需要修改参数的函数调用。
template <typename T>
class RefHolder {
	public:
		RefHolder(T& _ref) : ref(_ref) {}
		operator T& () const
		{
			return ref;
		}
	private:
		T& ref;
};

template <typename T>
inline RefHolder<T> ByRef(T& t)
{
	return RefHolder<T>(t);
}


// 根据C++标准，如果 const 的引用和被初始化为对一个临时变量的引用，
// 那么它会使这个临时变量的生命期变得和它自己一样。
typedef const ScopeGuardImplBase& ScopeGuard;

#endif //_SCOPE_GRUARD_H_

