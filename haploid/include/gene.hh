#ifndef _GENE_HH_
#define _GENE_HH_
#include <glob.hh>
#include <node.hh>

typedef std::shared_ptr<node> allele;
enum Type {SNP=0U,STR=1U,UNKNOWN};

class gene {
  private:
    Type 			_type;
    double 		_rate;
    uint32_t 	_length;

    allele 				  _root;
    std::vector<allele> _alleles;

  public:
    gene(void);
    gene(const gene&);
    gene& operator=(const gene&);
    ~gene(void);

    void rate(const double&);
    void length(const uint32_t&);
    void type(const Type&);

    double rate(void) const;
    uint32_t length(void) const;
    Type type(void) const;

    void insert(const allele&);
    void contract(void);
    void flush(void);

    allele create(void);
    allele random(void);

    void serialize(const std::string&) const;

    json save(void);
};
#endif
