" Vim syntax file
" SVN ID: $Id$
" Language: DataWorks
" Homepage: http://nishi.boats/dataworks
" Maintainer: Nishi <nishi@nishi.boats>

if exists("b:current_syntax")
	finish
endif

syn match dataworksString /\v"[^"]+"/ display
syn match dataworksString /\v'[^']+'/ display
syn match dataworksNumber /\v[+-]?[0-9]+(\.[0-9]+)?/ display
syn match dataworksComment /\v#.+$/ display
syn region dataworksArgument start='(' end=')' fold transparent
syn match dataworksIdent /\v\c(create_table|use|insert|version)/ display

hi def link dataworksString String
hi def link dataworksNumber Number
hi def link dataworksComment Comment
hi def link dataworksIdent Identifier

syn sync minlines=512
let b:current_syntax = "dataworks"
