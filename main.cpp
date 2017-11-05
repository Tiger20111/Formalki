#include <iostream>
#include <assert.h>
#include <algorithm>
#include <string>
#include <vector>


class Reg {
public:
    Reg(std::string s_, char x_, int k_) : str(s_), x(x_), k(k_) {}
    bool MinLen(int& answer); // return false if no word is needed

private:
    std::string str;
    char x;
    int k;
    struct PrefixSet {
        PrefixSet(int k_) : AllMinLen(k_ + 1, -1) {}

        std::vector<int> AllMinLen; //min length of word with prefix x^i, i <= k

        void NewMin(int degree, int len) {
            //set new min in massif AllMinLen[degree] equal len
            assert(0 <= degree && degree < AllMinLen.size());
            if (AllMinLen[degree] == -1) AllMinLen[degree] = len;
            else if (len >= 0) AllMinLen[degree] = std::min(AllMinLen[degree], len);
        }
    };

    std::vector<PrefixSet> stack;

    void Mul() {
        //joining two parts of a word into a word z
        PrefixSet z(k), x1(k), x2(k);
        x2 = stack.back();
        stack.pop_back();
        x1 = stack.back();
        stack.pop_back();

        //for all words in x1 and x2 match new word
        for (size_t i = 0; i < k + 1; i++) {
            for (size_t j = 0; j < k + 1; j++) {
                if (x1.AllMinLen[i] >= 0 && x2.AllMinLen[j] >= 0) {
                    int len = x1.AllMinLen[i] + x2.AllMinLen[j];
                    int pow = i;
                    if (x1.AllMinLen[i] == i) pow += j;
                    pow = std::min(pow, k);
                    z.NewMin(pow, len);
                }
            }
        }

        stack.push_back(z);
    }

    void Add() {
        //apply the operation between two regular expressions
        // (x1 and x2, the result is written in z)

        PrefixSet z(k), x1(k), x2(k);
        x2 = stack.back();
        stack.pop_back();
        x1 = stack.back();
        stack.pop_back();

        //for all words in x1 and x2 match new word
        for (size_t i = 0; i < k + 1; i++) {
            z.NewMin(i, x1.AllMinLen[i]);
            z.NewMin(i, x2.AllMinLen[i]);
        }

        stack.push_back(z);
    }

    void Pow() {
        //application * to the word or the entire regular expression in "(", ")"
        PrefixSet t = stack.back();
        stack.pop_back();

        t.AllMinLen[0] = 0;

        std::vector<int> suitable_words;//lengths of x^m words
        std::vector<std::pair<int, int>> inappropriate_words;//not x^m words.
        // First - length of x^m prefix, Second - length of word


        for (size_t i = 0; i < k + 1; i++) {
            if (t.AllMinLen[i] == i && i > 0) suitable_words.push_back(i);
            else inappropriate_words.push_back(std::make_pair((int)i, t.AllMinLen[i]));
        }

        std::vector<bool> saved_need(k + 1, false);
        //could we achieve x^i using only words from suitable_words

        for (int i = 0; i < suitable_words.size(); ++i) {
            saved_need[suitable_words[i]] = true;
        }

        for (int i = 0; i < k + 1; ++i) {
            if (saved_need[i]) {
                for (int j = 0; j < suitable_words.size(); ++j) {
                    if (i + suitable_words[j] < k + 1) {
                        saved_need[i + suitable_words[j]] = true;
                    } else {
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < k + 1; ++i) {
            if (saved_need[i]) {
                for (int j = 0; j < inappropriate_words.size(); ++j) {
                    if (i + inappropriate_words[j].first < k + 1 && inappropriate_words[j].second >= 0)
                        t.NewMin(i + inappropriate_words[j].first, i + inappropriate_words[j].second);
                }
            }
        }

        stack.push_back(t);
    }
};

bool Reg::MinLen(int& answer) {
    for (size_t i = 0; i < str.size(); i++) {
        char c = str[i];
        switch (c) {
        case '+':
            Add();
            break;
        case '*':
            Pow();
            break;
        case '.':
            Mul();
            break;
        case ' ':
            break;
        case '1': {
        PrefixSet lambda(k);
        lambda.AllMinLen[0] = 0;
        stack.push_back(lambda);
        }
            break;
        default:
        PrefixSet t(k);
            t.AllMinLen[0] = 1;
            if (c == x && k > 0) {
                t.AllMinLen[1] = 1;
            }
            stack.push_back(t);
            break;
        }
    }

    if (stack.size() > 1) return false;
    if (stack.back().AllMinLen[k] == -1) return false;
    answer = stack.back().AllMinLen[k];
    return true;
}

int main() {
    int ans;
    std::string s;
    char x;
    int k;
    std::getline(std::cin, s);
    std::cin >> x;
    std::cin >> k;
    Reg re(s, x, k);
    if (re.MinLen(ans)) std::cout << ans << std::endl;
    else std::cout << "INF\n";
}