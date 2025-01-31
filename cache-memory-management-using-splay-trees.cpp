#include <iostream>
#include <unordered_map>
#include <ctime>

using namespace std;

struct CacheEntry
{
    string key;
    string value;
    time_t timestamp;
    CacheEntry *left;
    CacheEntry *right;
};

class SplayTreeCache
{
private:
    int capacity;
    CacheEntry *splayTreeRoot;
    unordered_map<string, CacheEntry *> cacheMap;

    CacheEntry *newNode(string key, string value)
    {
        CacheEntry *newNode = new CacheEntry();
        newNode->key = key;
        newNode->value = value;
        newNode->timestamp = time(nullptr);
        newNode->left = nullptr;
        newNode->right = nullptr;

        return newNode;
    }

    CacheEntry *leftRotate(CacheEntry *root)
    {
        CacheEntry *newRoot = root->right;
        root->right = newRoot->left;
        newRoot->left = root;

        return newRoot;
    }

    CacheEntry *rightRotate(CacheEntry *root)
    {
        CacheEntry *newRoot = root->left;
        root->left = newRoot->right;
        newRoot->right = root;

        return newRoot;
    }

    CacheEntry *splay(CacheEntry *node, string key)
    {
        if (node == nullptr || node->key == key)
        {
            return node;
        }

        if (key < node->key)
        {
            if (node->left == nullptr)
                return node;

            if (key < node->left->key)
            {
                node->left->left = splay(node->left->left, key);
                node = rightRotate(node);
            }
            else if (key > node->left->key)
            {
                node->left->right = splay(node->left->right, key);
                if (node->left->right != nullptr)
                {
                    node->left = leftRotate(node->left);
                }
            }

            if (node->left == nullptr)
            {
                return node;
            }

            return rightRotate(node);
        }
        else
        {
            if (node->right == nullptr)
                return node;

            if (key > node->right->key)
            {
                node->right->right = splay(node->right->right, key);
                node = leftRotate(node);
            }
            else if (key < node->right->key)
            {
                node->right->left = splay(node->right->left, key);
                if (node->right->left != nullptr)
                {
                    node->right = rightRotate(node);
                }
            }

            if (node->right == nullptr)
            {
                return node;
            }
            return leftRotate(node);
        }
    }

    CacheEntry *find(CacheEntry *root, string key)
    {
        if (root == nullptr || root->key == key)
        {
            return root;
        }
        if (root->key > key)
        {
            return find(root->left, key);
        }
        else
        {
            return find(root->right, key);
        }
    }

    CacheEntry *insertNode(CacheEntry *root, string key, string value)
    {
        if (root == nullptr)
            return newNode(key, value);

        root = splay(root, key);

        if (root->key == key)
        {
            root->value = value;
            root->timestamp = time(nullptr);
            return root;
        }

        CacheEntry *newEntry = newNode(key, value);

        if (key < root->key)
        {
            newEntry->right = root;
            newEntry->left = root->left;
            root->left = nullptr;
        }

        else
        {
            newEntry->left = root;
            newEntry->right = root->right;
            root->right = nullptr;
        }

        return newEntry;
    }

    CacheEntry *findMaxiNode(CacheEntry *node)
    {
        if (node == nullptr)
            return node;

        while (node->right != nullptr)
        {
            node = node->right;
        }

        return node;
    }

    CacheEntry *eraseNode(CacheEntry *root, string key)
    {
        if (root == nullptr)
            return root;

        root = splay(root, key);

        if (root->key != key)
            return root;

        if (root->left == nullptr)
        {
            splayTreeRoot = root->right;
        }

        else
        {
            CacheEntry *tempNode = findMaxiNode(root->left);
            splayTreeRoot = root->left;
            tempNode->right = root->right;
        }

        delete root;
        cacheMap.erase(key);
        return splayTreeRoot;
    }

public:
    SplayTreeCache(int capacity)
    {
        this->capacity = capacity;
        splayTreeRoot = nullptr;
    }

    string get(string key)
    {
        CacheEntry *retrivedEntry = find(splayTreeRoot, key);
        if (retrivedEntry != nullptr && retrivedEntry->key == key)
        {
            retrivedEntry->timestamp = time(nullptr);
            splayTreeRoot = splay(splayTreeRoot, key);
            return retrivedEntry->value;
        }
        return "";
    }

    void put(string key, string value)
    {
        if (get(key) != "")
            return;

        if (cacheMap.size() >= capacity)
        {
            CacheEntry *LRU_node = findLeastRecentlyUsed(splayTreeRoot);
            splayTreeRoot = eraseNode(splayTreeRoot, LRU_node->key);
        }

        splayTreeRoot = insertNode(splayTreeRoot, key, value);
        cacheMap[key] = splayTreeRoot;
    }

    void remove(string key)
    {
        splayTreeRoot = eraseNode(splayTreeRoot, key);
    }

    void printCacheContents()
    {
        cout << "The Cache Contents are : " << endl;
        printCacheHelper(splayTreeRoot);
        cout << endl;
    }

private:
    void printCacheHelper(CacheEntry *root)
    {
        if (root == nullptr)
            return;

        printCacheHelper(root->left);
        cout << "Key : " << root->key << " ,Value : " << root->value << endl;
        printCacheHelper(root->right);
    }

    CacheEntry *findLeastRecentlyUsed(CacheEntry *root)
    {
        if(root == nullptr)return root;
        CacheEntry *LRU_Node = nullptr;
        time_t lru_time = time(nullptr);

        traverseTree(root, LRU_Node, lru_time);

        return LRU_Node;
    }

    void traverseTree(CacheEntry *root, CacheEntry *&lru_node, time_t &lru_time)
    {
        if (root == nullptr)
            return;

        traverseTree(root->left, lru_node, lru_time);
        if (root->timestamp < lru_time)
        {
            lru_node = root;
            lru_time = root->timestamp;
        }
        traverseTree(root->right, lru_node, lru_time);
    }
};

int main()
{
    int capacity;
    cout << "Enter the capacity of the cache : ";
    cin >> capacity;

    SplayTreeCache cache(capacity);

    char choice;
    do
    {
        cout << "\nPlease Select from the given option :\n";
        cout << "1. Add an entry \n";
        cout << "2. Retrive an entry \n";
        cout << "3. Remove an entry \n";
        cout << "4. Print contents of cache \n";
        cout << "5. Exit \n";
        cout << "Enter your choice : ";

        cin >> choice;

        string key, value;

        switch (choice)
        {

        case '1':
            cout << "Enter key : ";
            cin >> key;
            cout << "Enter value : ";
            cin >> value;
            cache.put(key, value);
            break;
        case '2':
            cout << "Enter the Key to search : ";
            cin >> key;
            value = cache.get(key);
            if (!value.empty())
            {
                cout << "Value :" << value << endl;
            }
            else
            {
                cout << "Entry not found in cache" << endl;
            }
            break;
        case '3':
            cout << "Enter the key to be removed : ";
            cin >> key;
            cache.remove(key);
            cout << "Entry removed from cache " << endl;
            break;
        case '4':
            cache.printCacheContents();
            break;
        case '5':
            cout << "Exiting program ..." << endl;
            break;

        default:
            cout << "Invalid choice, please try again ..." << endl;
            break;
        }

    } while (choice != '5');

    return 0;
}
