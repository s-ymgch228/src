#include <sys/types.h>
#include <sys/stat.h>

#include "debug.h"
#include "rtld.h"

void
_rtld_setup_pltgot(const Obj_Entry *obj)
{
	obj->pltgot[1] = (Elf_Addr) obj;
	obj->pltgot[2] = (Elf_Addr) &_rtld_bind_start;
}

int
_rtld_relocate_nonplt_objects(obj, dodebug)
	Obj_Entry *obj;
	bool dodebug;
{
	const Elf_Rel *rel;
#ifdef COMBRELOC
	unsigned long lastsym = -1;
#endif
	Elf_Addr target;

	for (rel = obj->rel; rel < obj->rellim; rel++) {
		Elf_Addr        *where;
		Elf_Addr         tmp;
		unsigned long	 symnum;

		where = (Elf_Addr *)(obj->relocbase + rel->r_offset);
		symnum = ELF_R_SYM(rel->r_info);

		switch (ELF_R_TYPE(rel->r_info)) {
		case R_TYPE(NONE):
			break;

#if 1 /* XXX should not occur */
		case R_TYPE(PC32):
#ifdef COMBRELOC
			if (symnum != lastsym)
#endif
			{
				const Elf_Sym   *def;
				const Obj_Entry *defobj;
				def = _rtld_find_symdef(symnum, obj, &defobj,
				    false);
				if (def == NULL)
					return -1;
				target = (Elf_Addr)(defobj->relocbase +
				    def->st_value);
#ifdef COMBRELOC
				lastsym = symnum;
#endif
			}

			*where += target - (Elf_Addr)where;
			rdbg(dodebug, ("PC32 %s in %s --> %p in %s",
			    defobj->strtab + def->st_name, obj->path,
			    (void *)*where, defobj->path));
			break;

		case R_TYPE(GOT32):
#endif
		case R_TYPE(32):
		case R_TYPE(GLOB_DAT):
#ifdef COMBRELOC
			if (symnum != lastsym)
#endif
			{
				const Elf_Sym   *def;
				const Obj_Entry *defobj;
				def = _rtld_find_symdef(symnum, obj, &defobj,
				    false);
				if (def == NULL)
					return -1;
				target = (Elf_Addr)(defobj->relocbase +
				    def->st_value);
#ifdef COMBRELOC
				lastsym = symnum;
#endif
			}

			tmp = target + *where;
			if (*where != tmp)
				*where = tmp;
			rdbg(dodebug, ("32/GLOB_DAT %s in %s --> %p in %s",
			    defobj->strtab + def->st_name, obj->path,
			    (void *)*where, defobj->path));
			break;

		case R_TYPE(RELATIVE):
			*where += (Elf_Addr)obj->relocbase;
			rdbg(dodebug, ("RELATIVE in %s --> %p", obj->path,
			    (void *)*where));
			break;

		case R_TYPE(COPY):
			/*
			 * These are deferred until all other relocations have
			 * been done.  All we do here is make sure that the
			 * COPY relocation is not in a shared library.  They
			 * are allowed only in executable files.
			 */
			if (!obj->mainprog) {
				_rtld_error(
			"%s: Unexpected R_COPY relocation in shared library",
				    obj->path);
				return -1;
			}
			rdbg(dodebug, ("COPY (avoid in main)"));
			break;

		default:
			rdbg(dodebug, ("sym = %lu, type = %lu, offset = %p, "
			    "contents = %p, symbol = %s",
			    symnum, (u_long)ELF_R_TYPE(rel->r_info),
			    (void *)rel->r_offset, (void *)*where,
			    obj->strtab + obj->symtab[symnum].st_name));
			_rtld_error("%s: Unsupported relocation type %ld "
			    "in non-PLT relocations\n",
			    obj->path, (u_long) ELF_R_TYPE(rel->r_info));
			return -1;
		}
	}
	return 0;
}
