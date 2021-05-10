#pragma once 
#include <string> // TODO: change usual pointer to uniuq_ptr and shared_ptr
#include <vector>
#include <memory>

namespace pt {

enum class NodesType { CONFIG, TABLE, TABLEBODY, TABLEITEM, IDENTIFIER, TYPE, VALUE }; // TODO:: add array

class Tree {
public:
    short mgetI8I16(std::string_view name);
    int mgetI32(std::string_view name);
    long mgetI64(std::string_view name);
    unsigned short mgetU8U16(std::string_view name);
    unsigned int mgetU32(std::string_view name);
    unsigned long mgetU64(std::string_view name);
    float mgetF32(std::string_view name);
    double mgetF64(std::string_view name);
    bool mgetBool(std::string_view name);
    std::string mgetString(std::string_view name);

    bool empty();

    Tree() = default;

    void m_addNode(const NodesType type, const std::string& value, bool isSameLevel = false);
    void m_addNode(const NodesType type, bool isSameLevel = false);

    void m_ascent();
private:
    struct Node{
        NodesType m_typeNode;
        std::string m_value;
        Node* m_parent;
        std::vector<Node*> m_children;
    };
    struct Value {
        std::string type;
        std::string value;
    };
    Node* m_root = nullptr;
    Node* m_current = m_root;
    Node* m_previous = nullptr;

    //Tree(const Tree& tree) = delete; // TODO: need copy constructor
    //Tree(const Tree&& tree) = delete; // TODO: need move constructor

    Tree::Node* m_createNode(const NodesType type);
    Tree::Node* m_createNode(const NodesType type, const std::string& value);
    Value m_recursiveDescent(Node& node, std::string_view name);
    
};

}