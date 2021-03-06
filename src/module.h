
// Compiler implementation of the D programming language
// Copyright (c) 1999-2013 by Digital Mars
// All Rights Reserved
// written by Walter Bright
// http://www.digitalmars.com
// License for redistribution is by either the Artistic License
// in artistic.txt, or the GNU General Public License in gnu.txt.
// See the included readme.txt for details.

#ifndef DMD_MODULE_H
#define DMD_MODULE_H

#ifdef __DMC__
#pragma once
#endif /* __DMC__ */

#include "root.h"
#include "dsymbol.h"

class ClassDeclaration;
struct ModuleDeclaration;
struct Macro;
struct Escape;
class VarDeclaration;
class Library;

// Back end
#ifdef IN_GCC
typedef union tree_node elem;
#else
struct elem;
#endif

enum PKG
{
    PKGunknown, // not yet determined whether it's a package.d or not
    PKGmodule,  // already determined that's an actual package.d
    PKGpackage, // already determined that's an actual package
};

class Package : public ScopeDsymbol
{
public:
    PKG isPkgMod;
    Module *mod;        // != NULL if isPkgMod == PKGmodule

    Package(Identifier *ident);
    const char *kind();

    static DsymbolTable *resolve(Identifiers *packages, Dsymbol **pparent, Package **ppkg);

    Package *isPackage() { return this; }

    virtual void semantic(Scope *) { }
    Dsymbol *search(Loc loc, Identifier *ident, int flags = IgnoreNone);
    void accept(Visitor *v) { v->visit(this); }
};

class Module : public Package
{
public:
    static Module *rootModule;
    static DsymbolTable *modules;       // symbol table of all modules
    static Modules amodules;            // array of all modules
    static Dsymbols deferred;   // deferred Dsymbol's needing semantic() run on them
    static Dsymbols deferred3;  // deferred Dsymbol's needing semantic3() run on them
    static unsigned dprogress;  // progress resolving the deferred list
    static void init();

    static AggregateDeclaration *moduleinfo;


    const char *arg;    // original argument name
    ModuleDeclaration *md; // if !NULL, the contents of the ModuleDeclaration declaration
    File *srcfile;      // input source file
    File *objfile;      // output .obj file
    File *hdrfile;      // 'header' file
    File *symfile;      // output symbol file
    File *docfile;      // output documentation file
    unsigned errors;    // if any errors in file
    unsigned numlines;  // number of lines in source file
    int isDocFile;      // if it is a documentation input file, not D source
    int needmoduleinfo;

    int selfimports;            // 0: don't know, 1: does not, 2: does
    int selfImports();          // returns !=0 if module imports itself

    int insearch;

    Module *importedFrom;       // module from command line we're imported from,
                                // i.e. a module that will be taken all the
                                // way to an object file

    Dsymbols *decldefs;         // top level declarations for this Module

    Modules aimports;             // all imported modules

    unsigned debuglevel;        // debug level
    Strings *debugids;      // debug identifiers
    Strings *debugidsNot;       // forward referenced debug identifiers

    unsigned versionlevel;      // version level
    Strings *versionids;    // version identifiers
    Strings *versionidsNot;     // forward referenced version identifiers

    Macro *macrotable;          // document comment macros
    Escape *escapetable;        // document comment escapes
    bool safe;                  // true if module is marked as 'safe'

    size_t nameoffset;          // offset of module name from start of ModuleInfo
    size_t namelen;             // length of module name in characters

    Module(const char *arg, Identifier *ident, int doDocComment, int doHdrGen);
    static Module* create(const char *arg, Identifier *ident, int doDocComment, int doHdrGen);

    static Module *load(Loc loc, Identifiers *packages, Identifier *ident);

    const char *kind();
    File *setOutfile(const char *name, const char *dir, const char *arg, const char *ext);
    void setDocfile();
    bool read(Loc loc); // read file, returns 'true' if succeed, 'false' otherwise.
    void parse();       // syntactic parse
    void importAll(Scope *sc);
    void semantic();    // semantic analysis
    void semantic2();   // pass 2 semantic analysis
    void semantic3();   // pass 3 semantic analysis
    void genobjfile(int multiobj);
    void gensymfile();
    int needModuleInfo();
    Dsymbol *search(Loc loc, Identifier *ident, int flags = IgnoreNone);
    void deleteObjFile();
    static void addDeferredSemantic(Dsymbol *s);
    static void runDeferredSemantic();
    static void addDeferredSemantic3(Dsymbol *s);
    static void runDeferredSemantic3();
    int imports(Module *m);

    bool isRoot() { return this->importedFrom == this; }
                                // true if the module source file is directly
                                // listed in command line.

    // Back end

    int doppelganger;           // sub-module
    Symbol *cov;                // private uint[] __coverage;
    unsigned *covb;             // bit array of valid code line numbers

    Symbol *sictor;             // module order independent constructor
    Symbol *sctor;              // module constructor
    Symbol *sdtor;              // module destructor
    Symbol *ssharedctor;        // module shared constructor
    Symbol *sshareddtor;        // module shared destructor
    Symbol *stest;              // module unit test

    Symbol *sfilename;          // symbol for filename

    Symbol *massert;            // module assert function
    Symbol *toModuleAssert();   // get module assert function

    Symbol *munittest;          // module unittest failure function
    Symbol *toModuleUnittest(); // get module unittest failure function

    Symbol *marray;             // module array bounds function
    Symbol *toModuleArray();    // get module array bounds function

    Symbol *toSymbol();
    void genmoduleinfo();

    Module *isModule() { return this; }
    void accept(Visitor *v) { v->visit(this); }
};


struct ModuleDeclaration
{
    Loc loc;
    Identifier *id;
    Identifiers *packages;            // array of Identifier's representing packages
    bool safe;

    ModuleDeclaration(Loc loc, Identifiers *packages, Identifier *id, bool safe);

    char *toChars();
};

#endif /* DMD_MODULE_H */
