let g:for_c=0

set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'gmarik/Vundle.vim'

" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
Plugin 'tpope/vim-fugitive'
" plugin from http://vim-scripts.org/vim/scripts.html
Plugin 'L9'
" Git plugin not hosted on GitHub
"Plugin 'git://git.wincent.com/command-t.git'
" git repos on your local machine (i.e. when working on your own plugin)
"Plugin 'file:///home/gmarik/path/to/plugin'
" The sparkup vim script is in a subdirectory of this repo called vim.
" Pass the path to set the runtimepath properly.
Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}
" Avoid a name conflict with L9
" Plugin 'user/L9', {'name': 'newL9'}




" 888888888888888888888888888888


" " Displays tags in a window, ordered by class etc, i used it instead of
" taglist
Bundle 'majutsushi/tagbar'
"
"

Bundle 'genutils'
"Bundle 'lookupfile'
"
"
" " Fuzzy file, buffer, mru, tag, ... finder with regexp support.
" Bundle 'kien/ctrlp.vim'
"
"
" " Fast file navigation
" Bundle 'wincent/Command-T'
"
"
" " Preview the definition of variables or functions in a preview window
" Bundle 'autopreview'
"
"
" " Echo the function declaration in the command line for C/C++
" Bundle 'echofunc.vim'
"
"
Bundle 'FuzzyFinder'
Bundle 'grep.vim'
Bundle 'a.vim'
Bundle 'taglist.vim'
Bundle 'The-NERD-Commenter'
Bundle 'The-NERD-tree'

Bundle 'wombat256.vim'

Bundle 'vim-ruby/vim-ruby'
"
"
" " Under linux need exec 'dos2unix
" ~/.vim/bundle/QFixToggle/plugin/qfixtoggle.vim'
" Bundle 'QFixToggle'
"
"
" Bundle 'Color-Sampler-Pack'
" Bundle 'altercation/vim-colors-solarized'
" Bundle 'txt.vim'
" Bundle 'mru.vim'
" Bundle 'YankRing.vim'
" Bundle 'tpope/vim-surround.git'
" Bundle 'DoxygenToolkit.vim'
" Bundle 'headerGatesAdd.vim'
" Bundle 'ShowMarks'
" Bundle 'Lokaltog/vim-powerline'
"
"
" " Deal with pairs of punctuations such as (), [], {}, and so on
"Bundle 'kana/vim-smartinput'
"Bundle "MarcWeber/vim-addon-mw-utils"
"Bundle "tomtom/tlib_vim"
"Bundle "honza/snipmate-snippets" 不能使用

"Bundle "garbas/vim-snipmate"

"for coffeescript
Bundle 'kchmck/vim-coffee-script'
















" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList          - list configured plugins
" :PluginInstall(!)    - install (update) plugins
" :PluginSearch(!) foo - search (or refresh cache first) for foo
" :PluginClean(!)      - confirm (or auto-approve) removal of unused plugins
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this lineßß

