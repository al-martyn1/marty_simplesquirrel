@set UINT8=--underlaying-type=std::uint8_t
@set UINT16=--underlaying-type=std::uint16_t
@set UINT32=--underlaying-type=std::uint32_t
@set INT=--underlaying-type=int

@set HEX2=--hex-width=2
@set HEX4=--hex-width=4
@set HEX6=--hex-width=6
@set HEX8=--hex-width=8

@rem set EXTRA=--enum-flags=extra
@rem same but more detailed
@set FLAGENUM_EXTRA=--enum-flags=serialize-set,deserialize-set

@set FLAGS=--enum-flags=flags
@set DECL=--enum-flags=type-decl
@set CLS=--enum-flags=enum-class
@set DECLCLS=--enum-flags=type-decl,enum-class
@set FMTHEX=--enum-flags=fmt-hex
@set SERIALIZE=--enum-flags=serialize,deserialize,lowercase

@set VALUES_CAMEL=--enum-values-style=CamelStyle
@set VALUES_PASCAL=--enum-values-style=PascalStyle 
@set SERIALIZE_PASCAL=--serialize-style=PascalStyle 


@set ENUMSCRIPTGENERATIONTYPE_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set ENUMSCRIPTGENERATIONTYPE_DEF=invalid,unknown=-1;singleLineScript=0;multiLineScript;mdDoc;trTemplate

@set LSTNESTEDTAGSMODE_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set LSTNESTEDTAGSMODE_DEF=invalid,unknown=-1;keep;remove;empty,emptyLine

@set APIDEFDIRECTIVE_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set APIDEFDIRECTIVE_DEF=invalid,unknown=-1;namespace=0;members;functions;classes;class


umba-enum-gen %GEN_OPTS% %HEX2% %TPL_OVERRIDE% ^
%ENUMSCRIPTGENERATIONTYPE_GEN_FLAGS%           %UINT32% -E=EnumScriptGenerationType          -F=%ENUMSCRIPTGENERATIONTYPE_DEF%    ^
%LSTNESTEDTAGSMODE_GEN_FLAGS%                  %UINT32% -E=ListingNestedTagsMode             -F=%LSTNESTEDTAGSMODE_DEF%           ^
%APIDEFDIRECTIVE_GEN_FLAGS%                    %UINT32% -E=ApiDefDirective                   -F=%APIDEFDIRECTIVE_DEF%             ^
..\enums.h


