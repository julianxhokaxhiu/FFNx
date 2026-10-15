// Freestanding harness: the CRT would allocate heaps in the retail exe's
// fixed address range. Only kernel32 is imported; no game process is touched.
#include "harness_support.h"
// Occupy the retail address range at process load, before Windows allocates heaps.
extern "C" uint8_t retail_image_space[0x2C00000];
#include "src/ff8/kernel_ability.cpp"

extern "C" void* __cdecl memcpy(void* to, const void* from, size_t n) {
 auto d = (uint8_t*)to; auto s = (const uint8_t*)from;
 for (size_t i=0; i<n; ++i) d[i]=s[i]; return to;
}
extern "C" void* __cdecl memset(void* to, int value, size_t n) {
 auto d=(uint8_t*)to; for(size_t i=0;i<n;++i) d[i]=(uint8_t)value; return to;
}
extern "C" int __cdecl memcmp(const void* a,const void* b,size_t n) {
 auto x=(const uint8_t*)a; auto y=(const uint8_t*)b;
 for(size_t i=0;i<n;++i) if(x[i]!=y[i]) return x[i]-y[i]; return 0;
}
static void print_args(const char* fmt, va_list args) {
 char out[2048]; unsigned used=0;
 for(unsigned i=0;fmt[i] && used<1900;++i) {
  if(fmt[i]!='%') {out[used++]=fmt[i];continue;}
  ++i; unsigned width=0;
  while(fmt[i]>='0'&&fmt[i]<='9') {width=width*10+fmt[i]-'0';++i;}
  if(fmt[i]=='s') {auto s=va_arg(args,const char*); while(*s && used<1900)out[used++]=*s++;}
  else if(fmt[i]=='c') out[used++]=(char)va_arg(args,int);
  else if(fmt[i]=='%')out[used++]='%';
  else {
   unsigned v=va_arg(args,unsigned); unsigned base=fmt[i]=='X'||fmt[i]=='x'?16:10;
   if(fmt[i]=='d'&&(int)v<0){out[used++]='-';v=-(int)v;}
   char digits[32];unsigned n=0;do{digits[n++]="0123456789ABCDEF"[v%base];v/=base;}while(v);
   while(n<width)digits[n++]='0';while(n)out[used++]=digits[--n];
  }
 }
 DWORD wrote;WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),out,used,&wrote,nullptr);
}
int audit_printf(const char* fmt,...) {va_list args;va_start(args,fmt);print_args(fmt,args);va_end(args);return 0;}
void ffnx_trace(const char* fmt,...) {++trace_count;va_list args;va_start(args,fmt);print_args(fmt,args);va_end(args);}
void ffnx_warning(const char* fmt,...) {va_list args;va_start(args,fmt);print_args(fmt,args);va_end(args);}
static void require(bool ok,const char* what) {
 audit_printf("%s: %s\n",ok?"PASS":"FAIL",what);if(!ok)ExitProcess(1);
}
struct FileBytes {uint8_t* data;DWORD size;};
static FileBytes read_file(const char* path) {
 HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
 require(f!=INVALID_HANDLE_VALUE,path);DWORD size=GetFileSize(f,nullptr),got=0;
 auto data=(uint8_t*)VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
 require(data!=nullptr,"Allocate input buffer");
 require(ReadFile(f,data,size,&got,nullptr)&&got==size,"Read entire input file");CloseHandle(f);return {data,size};
}
static uint32_t test_gf_save=0x1CFDCA8, test_gf_runtime=0x1CFF618;
static uint32_t test_text_end=0xB69000;
static void map_exe(FileBytes file) {
 DWORD initial_old;require(VirtualProtect(retail_image_space,sizeof(retail_image_space),PAGE_EXECUTE_READWRITE,&initial_old),"Writable private retail image");
 if(file.size>=12 && !memcmp(file.data,"FF8MAP01",8)) {
  unsigned segments=*(uint32_t*)(file.data+8), offset=12;
  for(unsigned i=0;i<segments;++i) {
   require(offset+8<=file.size,"Map segment header bounds");
   uint32_t address=*(uint32_t*)(file.data+offset), size=*(uint32_t*)(file.data+offset+4);offset+=8;
   require(address>=0x401000 && address+size>=address && address+size<=0x401000+sizeof(retail_image_space) && size<=file.size-offset,"Map segment bounds");
   memcpy((void*)address,file.data+offset,size);offset+=size;
  }
  require(offset==file.size,"Map consumed exactly");return;
 }

 auto dos=(IMAGE_DOS_HEADER*)file.data;auto nt=(IMAGE_NT_HEADERS32*)(file.data+dos->e_lfanew);
 require(nt->OptionalHeader.ImageBase==0x400000,"Expected retail image base");
 auto base=(uint8_t*)0x400000;
 require((uint32_t)retail_image_space==0x401000,"Harness padding owns retail sections");
 DWORD old;require(VirtualProtect(retail_image_space,sizeof(retail_image_space),PAGE_EXECUTE_READWRITE,&old),"Writable private retail image");
 auto sec=IMAGE_FIRST_SECTION(nt);
 for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i)if(sec[i].SizeOfRawData)
  memcpy(base+sec[i].VirtualAddress,file.data+sec[i].PointerToRawData,sec[i].SizeOfRawData);
}
static FileBytes audit_kernel_input;
static uint32_t audit_read_kernel(char*,char* dest) {
 memcpy(dest,audit_kernel_input.data,audit_kernel_input.size);return audit_kernel_input.size;
}
static void* driver_malloc(size_t size) {return VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);}
static unsigned magic_arm_count=0;
static void ff8_kernel_magic_arm() {++magic_arm_count;}
#include "production_kernel_loader.h"
static void load_kernel(FileBytes file) {
 audit_kernel_input=file;ff8_externals.sm_pc_read=audit_read_kernel;
 require(ff8_kernel_load_hook("kernel.bin",ff8_externals.unk_1CF3E48)==VANILLA_KERNEL_SIZE,"Actual shared kernel loader produces the vanilla image");
 auto offsets=(uint32_t*)file.data+1;
 unsigned magic_count=(offsets[2]-offsets[1])/MAGIC_ENTRY_SIZE;
 require(magic_arm_count==(magic_count>VANILLA_MAGIC_COUNT?1u:0u),"Ability-only loads do not arm the magic extension");
}

static char* next_arg(char*& cursor) {
 while(*cursor==' ')++cursor;bool quoted=*cursor=='"';if(quoted)++cursor;char* arg=cursor;
 while(*cursor&&(quoted?*cursor!='"':*cursor!=' '))++cursor;if(*cursor)*cursor++=0;return arg;
}

static uint8_t menu_task[128];
static void* __cdecl menu_task_alloc_stub(void*, void*) { return menu_task; }
static int __cdecl menu_handler_stub(int) { return 0; }
static void test_replace(uint32_t address, void* function) {
 *(uint8_t*)address=0xE9;*(uint32_t*)(address+1)=(uint32_t)function-address-5;
}
extern "C" void __cdecl audit_start() {
 require((uint32_t)retail_image_space==0x401000,"Retail address range reserved by executable image");
 char command[2048];lstrcpynA(command,GetCommandLineA(),2048);char* cursor=command;
 next_arg(cursor);auto exe_path=next_arg(cursor);auto kernel_path=next_arg(cursor);auto scenario=next_arg(cursor);auto config_path=next_arg(cursor);auto operands_path=next_arg(cursor);
 require(*exe_path&&*kernel_path,"Command supplies exe and kernel paths");
 auto exe=read_file(exe_path);auto kernel=read_file(kernel_path);map_exe(exe);
 if(*config_path) {
  auto config=read_file(config_path);require(config.size==10*4,"Language anchor record size");auto v=(uint32_t*)config.data;
  version=v[0];ff8_externals.unk_1CF3E48=(char*)v[1];ff8_externals.menu_use_items_sub_4F81F0=v[2];
  ff8_externals.sub_48B7E0=v[3];ff8_externals.compute_char_stats_sub_495960=v[4];test_magic_name_getter=v[5];
  test_gf_save=v[6];test_gf_runtime=v[7];test_text_end=v[8];
 }
 ff8_externals.savemap=(savemap_ff8*)(test_gf_save- offsetof(savemap_ff8,gfs));
 auto original_build=get_relative_call(get_relative_call(ff8_externals.menu_use_items_sub_4F81F0,layout_of(version).menu_mask_call),0x24);
 auto original_refresh=original_build+layout_of(version).call_shop_list-0x110;
 auto old_menu_buffer=get_absolute_value(original_refresh,0xD);
 auto original_chara_lists=original_build+layout_of(version).chara_ability_lists;
 auto old_command_buffer=get_absolute_value(original_chara_lists,0x79)-1;
 ff8_kernel_ability_init();

 if(scenario[0]=='r'){trace_all=false;*(uint8_t*)(original_refresh+0x40+0x61)=0x84;} // unsupported menu builder
 if(scenario[0]=='m') {
  require(!ff8_kernel_ability_read((char*)kernel.data,(uint32_t*)kernel.data+1,kernel.size),"Magic-only kernel leaves ability extension off");
  load_kernel(kernel);
  require(!ff8_ability_armed&&ff8_magic_count==58&&magic_arm_count==1,"Magic-only extension retains the upstream load path");
  require(get_absolute_value(test_magic_name_getter-0x260,0xD)==(uint32_t)ff8_externals.unk_1CF3E48+ABILITY_BLOCK_OFFSET,"Magic-only load leaves ability readers untouched");
  ExitProcess(0);
 }
 bool armed=ff8_kernel_ability_read((char*)kernel.data,(uint32_t*)kernel.data+1,kernel.size);
 if(scenario[0]=='v'||scenario[0]=='r'||scenario[0]=='k') {
  require(!armed,scenario[0]=='v'?"Vanilla kernel leaves extension off":"Unknown menu encoding disables extension");
  require(get_absolute_value(test_magic_name_getter-0x260,0xD)==(uint32_t)ff8_externals.unk_1CF3E48+ABILITY_BLOCK_OFFSET,"Rejected or vanilla load leaves ability reader untouched");
  require(get_absolute_value(original_refresh,0xD)==old_menu_buffer,"Rejected or vanilla load leaves menu buffer untouched");
  require(!ff8_ability_armed,"Rejected or vanilla file does not arm patches");
  if(scenario[0]=='r') require(trace_count==0,"Unsupported mod diagnostics stay silent without trace_all");
  ExitProcess(0);
 }
 require(armed,"Production ability loader accepts kernel on installed executable");
 require(ff8_seal_row_ok&&ff8_seal_list_ok,"Both map-seal command functions validate on this language");
 load_kernel(kernel);memset((void*)test_gf_save,0,16*68);
 auto level_address=get_absolute_value(ability_ext.fn_build_list,0xB7);*(uint8_t*)level_address=1;
 uint8_t list[22][8]={};auto builder=(int(__cdecl*)(int,uint8_t*,int))ability_ext.fn_build_list;
 int count=builder(0,&list[0][0],1);require(count>0&&count<=22,"Actual game builds fresh Quezacotl learn list");
 auto name=(const uint8_t*(__cdecl*)(int))ability_ext.fn_get_name;
 audit_printf("LEARN_LIST_COUNT=%d\n",count);
 for(int i=0;i<count;++i) {
  auto text=name(list[i][0]);audit_printf("ABILITY id=%u slot=%u state=%u group=%u AP=%u text_hex=",
   list[i][0],list[i][1],list[i][2],list[i][3],list[i][4]);
  for(int n=0;n<100&&text[n];++n)audit_printf("%02X",text[n]);audit_printf("\n");
 }
 // A GF-only kernel ends with one additional GF ability. Test its learned
 // state and real battle-stat effect without touching any on-disk save.
 if(ff8_group_first[GROUP_GF]==83 && ff8_group_first[GROUP_MENU]==93) {
  const int added_gf_id=92;bool found=false;
  for(int i=0;i<count;++i)if(list[i][0]==added_gf_id)
   found=list[i][2]==1&&list[i][3]==5&&list[i][4]==10;
  require(found,"New GF ability is learnable at level 1 with 10 AP");
  auto stats=(int(__cdecl*)(int))ability_ext.fn_gf_battle_stats;
  stats(0);
  require(*(uint8_t*)(test_gf_runtime+9)==0,"Unlearned added GF ability gives no damage bonus");
  auto learned=(uint8_t*)(test_gf_save+20);learned[added_gf_id/8]|=1<<(added_gf_id%8);
  count=builder(0,&list[0][0],1);found=false;
  for(int i=0;i<count;++i)if(list[i][0]==added_gf_id)found=list[i][2]==2&&list[i][3]==5;
  require(found,"Learned new GF ability stays visible in the original game list");
  stats(0);
  require(*(uint8_t*)(test_gf_runtime+9)==50,"Original GF battle-stat code applies added SumMag +50% effect");
 }
 auto menu=ability_ext.fn_junction_menu;
 require(get_absolute_value(menu,0x5186)==(uint32_t)ff8_command_candidates+1
  &&get_absolute_value(menu,0x51E4)==(uint32_t)ff8_passive_candidates+1,
  "Both junction-menu rebuilds target relocated candidate lists");
 audit_printf("ABILITY_TOTAL=%d GROUP_FIRST_IDS=",ff8_ability_count);for(auto id:ff8_group_first)audit_printf("%u ",id);audit_printf("\n");

 unsigned stale_array=0,stale_lists=0;
 auto check_operand=[&](uint32_t address) {
  uint32_t value=*(uint32_t*)address;
  if(value>=(uint32_t)ff8_externals.unk_1CF3E48+ABILITY_BLOCK_OFFSET&&value<(uint32_t)ff8_externals.unk_1CF3E48+ABILITY_BLOCK_OFFSET+VANILLA_ABILITY_COUNT*ABILITY_ENTRY_SIZE){++stale_array;audit_printf("STALE_ARRAY operand=%08X value=%08X\n",address,value);}
  if((value>=old_command_buffer&&value<old_command_buffer+136)||(value>=old_menu_buffer&&value<old_menu_buffer+24)){++stale_lists;audit_printf("STALE_LIST operand=%08X value=%08X\n",address,value);}

 };
 if(*operands_path) {
  auto file=read_file(operands_path);require(file.size>=4,"Relocation operand file header");auto entries=(uint32_t*)file.data;
  require(entries[0]==(file.size-4)/4,"Relocation operand file bounds");
  for(unsigned i=0;i<entries[0];++i)check_operand(entries[i+1]);
 } else {
  for(uint32_t address=0x401000;address+4<=test_text_end;++address)check_operand(address);
 }
 require(stale_array==0&&stale_lists==0,"All old ability and menu list operands are repointed");
 bool initialize=scenario[0]=='i';
 bool all=scenario[0]=='a'||(initialize&&scenario[5]=='a');
 bool none=scenario[0]=='n';
 bool last=scenario[0]=='l';
 unsigned first=ff8_group_first[GROUP_MENU], menu_size=ff8_ability_count-first;
 unsigned chosen=last?ff8_ability_count-1:first;
 memset((void*)test_gf_save,0,16*68);
 unsigned expected=0;
 if(!none) {
  for(unsigned i=0;i<(all?menu_size:1);++i) {
   unsigned id=all?first+i:chosen;
   unsigned gf=all?i/16:0;
   *(uint8_t*)(test_gf_save+gf*68+17)=1;
   auto learned=(uint8_t*)(test_gf_save+gf*68+20);
   learned[id/8]|=1<<(id%8);
   expected|=1u<<(id-first);
  }
 }
 ((void(__cdecl*)())ability_ext.fn_menu_mask)();
 require(*ability_ext.learned_menu_mask==expected,"Learned menu capability mask is correct, including bit 31");
 memset((void*)old_menu_buffer,0xA5,24);
 *(uint16_t*)(old_menu_buffer+26)=0x5A5A;
 if(initialize) {
  uint32_t init=ability_ext.fn_menu_list_init;
  test_replace(get_relative_call(init,0x0C),(void*)menu_task_alloc_stub);
  test_replace(ability_ext.fn_call_shop_list,(void*)menu_handler_stub);
  ((void*(__cdecl*)())init)();
 } else {
  ((char(__cdecl*)())ability_ext.fn_menu_list_refresh)();
 }
 auto menu_list=*(uint8_t**)(ability_ext.fn_menu_list_refresh+0x00D);
 auto initial_list=*(uint8_t**)(ability_ext.fn_menu_list_init+0x040);
 unsigned actual_count=*(uint8_t*)(old_menu_buffer+24);
 require(menu_list==initial_list&&menu_list!=(void*)old_menu_buffer,"Both builders share the relocated menu list");
 require(actual_count==(none?0:all?menu_size:1),"Menu list contains every learned menu ability");
 for(unsigned i=0;i<actual_count;++i)
  require(menu_list[i]==(all?first+i:chosen),"Menu list stores the shifted global ability id");
 for(unsigned i=0;i<24;++i)
  require(*(uint8_t*)(old_menu_buffer+i)==0xA5,"Retired menu buffer remains untouched");
 require(*(uint16_t*)(old_menu_buffer+26)==0x5A5A,"32 menu entries preserve adjacent game state");
 audit_printf("RESULT menu_first=%u menu_size=%u count=%u mask=%08X path=%s\n",
  first,menu_size,actual_count,expected,initialize?"initialization":"refresh");

 ExitProcess(0);
}
