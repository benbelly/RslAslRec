
#ifndef GLOSS_H__
#define GLOSS_H__

#include "SignSeq.h"
#include<string>
#include<memory>

class Gloss {
    public:
        Gloss( std::string g );
        virtual ~Gloss();

        // Add another sequence to this gloss
        SignSeq *NextSeq();

    private:
        std::string gloss;
        std::vector<std::shared_ptr<SignSeq> > trainedSeqs;
};

#endif
