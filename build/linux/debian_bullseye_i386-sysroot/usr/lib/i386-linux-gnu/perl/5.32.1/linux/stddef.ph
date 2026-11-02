require '_h2ph_pre.ph';

no warnings qw(redefine misc);

unless(defined(&_LINUX_STDDEF_H)) {
    eval 'sub _LINUX_STDDEF_H () {1;}' unless defined(&_LINUX_STDDEF_H);
    unless(defined(&__always_inline)) {
	eval 'sub __always_inline () { &__inline__;}' unless defined(&__always_inline);
    }
    eval 'sub __struct_group () {( &TAG,  &NAME,  &ATTRS,  &MEMBERS...) \'union union\' { 1; 1; };}' unless defined(&__struct_group);
}
1;
