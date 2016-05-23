#ifndef CHAT_DESCRIPTOR_H
#define CHAT_DESCRIPTOR_H

class Desciptor {
public:
    virtual ~Desciptor() {};
    virtual int getDescriptor() const = 0;
    virtual void destroy() = 0;

    virtual bool operator==(const Desciptor &rhs) const = 0;
    virtual bool operator!=(const Desciptor &rhs) const = 0;
    virtual bool operator<(const Desciptor &rhs) const = 0;
    virtual bool operator>(const Desciptor &rhs) const = 0;
};

#endif //CHAT_DESCRIPTOR_H
