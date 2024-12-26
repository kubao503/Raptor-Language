template <typename... Types>
class Visitor;

template <typename T>
class Visitor<T> {
   public:
    virtual void operator()(const T& visitable) = 0;
};

template <typename T, typename... Types>
class Visitor<T, Types...> : public Visitor<Types...> {
   public:
    using Visitor<Types...>::operator();

    virtual void operator()(const T& visitable) = 0;
};

template <typename... Types>
class ConstVisitor;

template <typename T>
class ConstVisitor<T> {
   public:
    virtual void operator()(const T& visitable) const = 0;
};

template <typename T, typename... Types>
class ConstVisitor<T, Types...> : public ConstVisitor<Types...> {
   public:
    using ConstVisitor<Types...>::operator();

    virtual void operator()(const T& visitable) const = 0;
};

template <typename Visitor>
struct Visitable {
    virtual void accept(Visitor& visitor) const = 0;
};

template <typename Derived, typename Visitor>
class VisitableImpl : public virtual Visitable<Visitor> {
   public:
    void accept(Visitor& visitor) const override {
        visitor(static_cast<const Derived&>(*this));
    }
};

template <typename Visitor>
struct ConstVisitable {
    virtual void accept(const Visitor& visitor) const = 0;
};

template <typename Derived, typename ConstVisitor>
class ConstVisitableImpl : public virtual ConstVisitable<ConstVisitor> {
   public:
    void accept(const ConstVisitor& visitor) const override {
        visitor(static_cast<const Derived&>(*this));
    }
};