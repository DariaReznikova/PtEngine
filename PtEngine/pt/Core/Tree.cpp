#include "Tree.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include <memory>

namespace pt {

short Tree::mgetI8I16(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return MAXI16;
    }
    else {
        if (search.type == "i8" || search.type == "i16") {
            long long v = atoi(search.value.c_str());
            return v;
        }
        else {
            return MAXI16;
        }
    }
}

unsigned short Tree::mgetU8U16(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return MAXU16;
    }
    else {
        if (search.type == "u8" || search.type == "u16") {
            long long v = atoi(search.value.c_str());
            return v;
        }
        else {
            return MAXU16;
        }
    }
}

int Tree::mgetI32(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return MAXI32;
    }
    else {
        if (search.type == "i32") {
            long long v = atoi(search.value.c_str());
            return v;
        }
        else {
            return MAXI32;
        }
    }
}

long Tree::mgetI64(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return MAXI64;
    }
    else {
        if (search.type == "i64") {
            long long v = atoi(search.value.c_str());
            return v;
        }
        else {
            return MAXI64;
        }
    }
}

unsigned int Tree::mgetU32(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return MAXU32;
    }
    else {
        if (search.type == "u32") {
            long long v = atoi(search.value.c_str());
            return v;
        }
        else {
            return MAXU32;
        }
    }
}

unsigned long Tree::mgetU64(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return MAXU64;
    }
    else {
        if (search.type == "u64") {
            long long v = atoi(search.value.c_str());
            return v;
        }
        else {
            return MAXU64;
        }
    }
}

float Tree::mgetF32(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return 0;
    }
    else {
        if (search.type == "f32") {
            float v = atof(search.value.c_str());
            return v;
        }
        else {
            return 0;
        }
    }
}

double Tree::mgetF64(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return 0;
    }
    else {
        if (search.type == "f64") {
            double v = atof(search.value.c_str());
            return v;
        }
        else {
            return 0;
        }
    }
}

/*bool Tree::getBool(std::string_view name) {
    Value search;
    search = recursive_descent((*m_root), name);
    if (search.value == "_N_O_T_") {
        return MAXI16;
    }
    else {
        if (search.type == "bool") {
            long long v = atoi(search.value.c_str());
            return v;
        }
        else {
        return MAXI16;
        }
    }
}*/

std::string Tree::mgetString(std::string_view name) {
    Value search;
    search = m_recursiveDescent((*m_root), name);
    if (search.value == "_N_O_T_") {
        search.value.clear();
        return search.value;
    }
    else {
        if (search.type == "string") {
            return search.value;
        }
        else {
            return search.value;
        }
    }
}

void Tree::m_addNode(const NodesType type, bool isSameLevel) {
    Node* newNode = m_createNode(type);
    if (m_root == nullptr) {
        m_root = newNode;
    }
    else {
        if (isSameLevel) {
                m_previous->m_children.push_back(newNode);
        }
        else {
            m_current->m_children.push_back(newNode);
        }
    }
    if (!isSameLevel) {
        m_previous = m_current;
    }
    m_current = newNode;
}

void Tree::m_addNode(const NodesType type, const std::string& value, bool isSameLevel) {
    Node* newNode = Tree::m_createNode(type, value);
    if (m_root == nullptr) {
        m_root = newNode;
    }
    else {
        if (isSameLevel) {
            m_previous->m_children.push_back(newNode);
        }
        else {
            m_current->m_children.push_back(newNode);
        }
    }
    if (!isSameLevel) {
        m_previous = m_current;
    }
    m_current = newNode;
}

Tree::Node* Tree::m_createNode(const NodesType type, const std::string& value) {
    Node* node = new Node;
    node->m_parent = m_current;
    node->m_typeNode = type;
    node->m_value = value;	
    return node;
}

Tree::Node* Tree::m_createNode(const NodesType type) {
    Node* node = new Node;
    node->m_parent = m_current;
    node->m_typeNode = type;
    node->m_value = "";
    return node;
}

bool Tree::empty() {
    if (m_root == nullptr) {
        return true;
    }
    return false;
}

void Tree::m_ascent() {
    m_current = m_previous;
    m_previous = m_previous->m_parent;
}

Tree::Value Tree::m_recursiveDescent(Node& node, std::string_view name) {
    static bool found = false;
    static Value value;
    if (!node.m_children.empty()) {
        for (Node* child : node.m_children) {
            if (found && child->m_typeNode == NodesType::VALUE) {
                value.value = (*child).m_value;
                return value;
            }
            else if (found) {
                value.type = (*child).m_value;
                continue;
            }
            if (child->m_value == name) {
                found = true;
                continue;
            }
            else {
                m_recursiveDescent((*child), name);
                if (found) { return value; }
            }
        }
    }
    else {
        value.type = "_N_O_T_";
        value.value = "_N_O_T_";
        return value;
    }
}


}