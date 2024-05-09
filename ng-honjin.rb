require 'victor'

#シフトは親指か？
THUMB = false

#本陣 20240428版
LAYOUT = [

# [
#     "へひとこほ★する★★",
#     "ーきんかのあいう★★",
#     "そちてしもなった★★",
# ], [
#     "みわりふ小★え★★★",
#     "ませさはけゆよ★★★",
#     "ねめつくにやお★★★",
# ], [
#     "★★★★★★★★★★",
#     "む★られを★★★★★",
#     "ぬ★ろ★★★★★★★",
# ]

[
    "へひとこほ★する★★",
    "ーけんかくあいう★★",
    "そちてしきなった★★",
], [
    "みわりふ小★え★★★",
    "ませさはつゆよ★★★",
    "ねめられにやお★★★",
], [
    "★★★★★★★★★★",
    "む★ものを★★★★★",
    "ぬ★ろ★★★★★★★",
]

]



INACTIVE = [
    # "くぃ", "くゎ","ぐぁ", 
    "ふゅ", "ゔゅ"
]

CUT_RATE=0.5

# SHIFT_KEYS = ["B_SHFT","B_SHFT2"]
# SHIFT_ORDER = [[],["B_SHFT"],["B_SHFT2"],["B_SHFT","B_SHFT2"]]

# SHIFT_KEYS = ["B_D","B_K","B_C","B_COMM"]
# SHIFT_ORDER = [[],["B_D"],["B_K"],["B_C"],["B_COMM"]]

SHIFT_KEYS = ["B_K","B_J"]
SHIFT_ORDER = [[],["B_K"],["B_J"]]


#修飾キー
def mod_key(w,side)
    mod_keys = {
        "濁音"   => {:left=>"B_SHFT", :right=>"B_SHFT"} ,
        "半濁音" => {:left=>"B_SHFT2", :right=>"B_SHFT2"} ,
        # "濁音"   => {:left=>"B_K", :right=>"B_D"} ,
        # "半濁音" => {:left=>"B_I", :right=>"B_E"} ,
        # "濁音"   => {:left=>"B_SHFT2", :right=>"B_SHFT"} ,
        # "半濁音" => {:left=>"B_SHFT", :right=>"B_SHFT2"} ,
    }
    mod_keys[w] ? mod_keys[w][side] : nil
end

#特殊なキーの組み合わせ
EXCEPTIONAL_KEY_SETS = {
    # "どぅ" => ["B_E","B_K","B_H"],
    # "とぅ" => ["B_E","B_K","B_N"]
}


#ソース
SRC=[
    "keyboards/dailycraft/claw44/keymaps/frym/honjin.c",
    # "keyboards/takashicompany/endzone34/keymaps/frym/naginata_frym.c"
]

#ソースの更新
def update_src(codes_array)
    SRC.each{|file|
        src=[]
        is_src=true
        f = File.open(file,"r")
        while line=f.gets
            is_src=true if line=~/\/\/ 追加/
            if is_src
                src.push(line)
                if line=~/const PROGMEM naginata_keymap ngmap/
                    is_src = false
                    src = src + codes_array
                end
            end
        end 
        f.close
        f = File.open(file,"w")
        f.puts(src)
        f.close
    }
end

#評価用テキストの読み込み
TEXT="../src-kana.txt"
def sentences(fingers_map)
    f = File.open(TEXT,"r")
    sentence_list = []
    sentence = []
    count=0
    while line=f.gets
        chars = line.split("")
        while chars.size > 0
            c= chars.slice(0,2).join("")
            if fingers_map[c]
                sentence.push(c)
                chars.slice!(0,2)
                count+=2
            else
                c=chars[0]
                if fingers_map[c]
                    sentence.push(c)
                    count+=1
                else
                    sentence_list.push(sentence)  if sentence.size>0
                    sentence = []
                end    
                chars.shift
            end
        end
        sentence_list.push(sentence) if sentence.size>0
        sentence=[]
    end
    puts "■全 #{count}文字"
    sentence_list
end

#評価
def eval_map(fingers_map)
    key_count = Hash.new{|h,k|h[k]=0}
    reap = Hash.new{|h,k|h[k]=0}
    updown = Hash.new{|h,k|h[k]=0}
    same = Hash.new{|h,k|h[k]=0}
    arpegio = Hash.new{|h,k|h[k]=0}
    triple = Hash.new{|h,k|h[k]=0}
    arp_fingers = Hash.new{|h,k|h[k]=0}
    long_seq = Hash.new{|h,k|h[k]=0}
    long_buf = []
    triple_seq = Hash.new{|h,k|h[k]=0}
    cnt = 0
    sentences(fingers_map).each{|sentence|
        last_keys=nil
        last_fingers=nil
        sentence.each{|kana|
            keys = fingers_map[kana][:keys]
            has_reap = false #同じ指、２段跳び
            has_same = false #同じキー連続
            has_arp  = false #別の指、連接
            has_updown = false #同じ指、上下隣接
            if last_keys
                chars = last_keys[:kana]+kana
                keys.each_index{|i|
                    k=keys[i]
                    # puts k
                    last_keys[:key].each_index{|li|
                        lk=last_keys[:key][li]
                        # puts lk
                        if lk[:side]==k[:side]
                            row_d = (lk[:row]-k[:row]).abs
                            col_d = (lk[:finger]-k[:finger]).abs
                            has_reap = true if row_d == 2 && col_d < 1.0
                            has_same = true if row_d == 0 && col_d == 0 && i==0 && li==0
                            if 0 < col_d && col_d < 2.0 && row_d < 2
                                has_arp  = true 
                            end
                            arp_fingers[[k[:side], [lk[:col],lk[:row]] ,[k[:col],k[:row]]]]+=1
                            has_updown = true if row_d == 1 && col_d < 1.0
                        end
                    }
                }
                reap[chars] += 1    if has_reap
                same[chars] += 1    if has_same
                updown[chars] += 1  if has_updown
                arpegio[chars] += 1 if has_arp
            end
            keys.each{|k|
                key_count[k[:code]]+=1
            }
            if long_buf.size==0 || has_arp || has_same
                long_buf.push(kana)
            else
                if long_buf.size>2
                    seqs=long_buf.join("")
                    long_seq[seqs]+=1
                    (0..seqs.size-3).each do |n|
                        triple_seq[seqs.slice(n,3)]+=1
                    end
                end
                long_buf = []
            end
            last_keys= {:kana=>kana, :key=>keys}
            cnt+=1
        }
        if long_buf.size>2
            seqs=long_buf.join("")
            long_seq[seqs]+=1
            (0..seqs.size-3).each do |n|
                triple_seq[seqs.slice(n,3)]+=1
            end
        end
        long_buf = []
    }
    r=CUT_RATE
    puts "■計 #{cnt}回"
    puts "■同指段飛び #{reap.keys.size}種 #{reap.values.sum}回 (#{reap.values.sum * 100 / cnt}%)"
    puts cut(reap,r)
    puts "■同指上下 #{updown.keys.size}種 #{updown.values.sum}回 (#{updown.values.sum * 100 / cnt}%)"
    puts cut(updown,r)
    puts "■同鍵連続 #{same.keys.size}種 #{same.values.sum}回 (#{same.values.sum * 100 / cnt}%)"
    puts cut(same,r)
    puts "■アルペジオ #{arpegio.keys.size}種 #{arpegio.values.sum}回 (#{arpegio.values.sum * 100 / cnt}%)"
    puts cut(arpegio,r)
    puts "■3連 #{triple_seq.keys.size}種 #{triple_seq.values.sum}回 (#{triple_seq.values.sum * 100 / cnt}%)"
    puts cut(triple_seq,r,1000)
    puts "■アルペジオ(3音以上) #{long_seq.keys.size}種 #{long_seq.values.sum}回"
    puts cut(long_seq,r/5.0)
    puts long_seq.keys.sort{|a,b|b.size<=>a.size}.map{|x|x.size}.uniq.join(", ")
    puts long_seq.keys.sort{|a,b|b.size<=>a.size}.slice(0,10).join("\n")
    puts "■打鍵数 #{key_count.values.sum}回"
    # puts key_count
    # puts LAYOUT.map{|board|
    #     board.map{|row|
    #         row.split("").map{|col| key_count[col] || 0 }.join("\t")
    #     }.join("\n")
    # }.join("\n")
    puts [
        "Q,W,E,R,T,x,Y,U,I,O,P",
        "A,S,D,F,G,x,H,J,K,L,SCLN",
        "Z,X,C,V,B,x,N,M,COMM,DOT,SLSH"
    ].map{|row|
        row.split(",").map{|col| key_count["B_"+col] || 0 }.join("\t")
    }.join("\n")
    draw_svg(arp_fingers)
end

def cut(h,rate,border=0)
    max = h.values.sum * rate
    sum = 0
    res = {}
    h.keys.sort{|a,b|h[b]<=>h[a]}.each {|k|
        res[k]=h[k] if sum < max && h[k]>=border
        sum += h[k]
    }
    res
end

#連接の画像
def draw_svg(moves)

    size = 100
    line_width = 20
    margin = 20
    w = size * 11 + margin * 2
    h = size * 3 + margin * 2

    svg = Victor::SVG.new width: w, height: h, style: { background: '#f0f0f0' }
    svg.build do
        marker id:"arrow", viewBox:"-5 -10 10 20", orient:"auto" ,markerUnits:"strokeWidth", markerWidth:"1.8", markerHeight:"1.8" do
            polygon points:"0,-10 5,0 0,10", stroke:"none", style:{fill:"#f00000", fill_opacity:"0.6"}
        end
        2.times do |side|
            5.times do |col|
                x=size*(side*6+col) + margin
                3.times do |row|
                    y=size*row + margin
                    g do
                        rect x: x, y:y, width:size, height:size, style:{stroke: '#808080', fill:'#f0f0f0'}
                        g style:{stroke:'#404040', fill:'#404040'} do 
                            text LAYOUT[0][row][col+side*5], x:x+10, y:y+30, font_size:20
                            text LAYOUT[1][row][col+side*5], x:x+10, y:y+50, font_size:16
                            text LAYOUT[2][row][col+side*5], x:x+30, y:y+50, font_size:16
                        end
                    end
                end
            end
        end
        
        g style:{stroke: '#f00000'}, marker_end:"url(#arrow)" do
            moves.keys.filter{|k|moves[k]>0}.each {|k|
                w = [moves[k].to_f / 1000, 0.08].max
                op = [moves[k].to_f / 100000, 0.025].max
                # op = 0.15
                side=k[0]
                from=k[1]
                to=k[2]
                y_base = margin  + size/2
                x_base = y_base  + (side == :right ? 6 : 0)*size
                x1 = x_base + from[0]*size
                x2 = x_base + to[0]*size 
                y1 = y_base + from[1]*size 
                y2 = y_base + to[1]*size 
                if x1 != x2 || y1 != y2
                    line x1:x1, y1:y1, x2:x2, y2:y2, style:{stroke_opacity: op, stroke_width: w}
                end
            }
        end
    end
    svg.save 'finger-moves-honjin'
end




#同列チェック
col_map={}

#qwerty
kbd_map=[
    "qwertyuiop",
    "asdfghjkl;",
    "zxcvbnm,./"
].map{|row|
    col=0
    row.split("").map{|c|
        c= {
            ";" => "SCLN",
            "," => "COMM",
            "." => "DOT",
            "/" => "SLSH"
        }[c] || c.upcase
        k="B_"+c
        col_map[k]=col
        col+=1
        k
    }
}

#キーコードから指の位置を返す
pos_map = {}
finger_index = [5.0,4.0,3.0,2.0,1.5,1.5,2.0,3.0,4.0,5.0]
kbd_map.each_index{|r|
    row = kbd_map[r]
    row.each_index{|c|
        code=row[c]
        side = c < 5 ? :left : :right
        finger = finger_index[c]
        extra = c==4 || c==5
        pos_map[code]={
            :code=>code,
            :side=>side, 
            :finger=>finger, 
            :extra=>extra,
            :row=>r,
            :col=>c<5 ? c : c-5
        }
    }
}
pos_map["B_SHFT"]={
    :code=>"B_SHFT",
    :side=>:both, 
    :finger=>1.0, 
    :extra=>false,
    :row=>3,
    :col=>0
}

pos_map["B_SHFT2"]={
    :code=>"B_SHFT2",
    :side=>:both, 
    :finger=>1.0, 
    :extra=>false,
    :row=>3,
    :col=>0
}



def build_layout(board)
    shift=0
    tbl={}
    board.each {|side|
        r=0
        side.each {|row|
            c=0
            row.split("").each{|chr|
                tbl[chr] = {
                    :shift => shift, 
                    :side  => c<5 ? :left : :right,
                    :row   => r,
                    :col   => c
                }
                c+=1
            }
            r+=1
        }
        shift += 1
    }
    tbl.delete("★")
    #
    tbl.each{|k,v|
        v[:next]=nil
        s = nil
        if v[:side] == :left
           s = v[:col]==4 ? 2 : v[:col]+1
        else
           s = v[:col]==5 ? 7 : v[:col]-1
        end
        other = tbl.keys.filter{|x|tbl[x][:shift]==v[:shift] && tbl[x][:row]==v[:row] && tbl[x][:col]==s}
        v[:next]= other[0] if other.size > 0
    }
    

    # 裏表２面の場合、シフト面の文字を設定
    # tbl.each{|k,v|
    #     v[:other]=nil
    #     if !v[:shift]
    #         other = tbl.keys.filter{|x|tbl[x][:shift] && tbl[x][:row]==v[:row] && tbl[x][:col]==v[:col]}
    #         v[:other]= other[0] if other.size > 0
    #         puts v.inspect
    #     end
    # }
    tbl
end

layout_map = build_layout(LAYOUT)
# puts layout_map.inspect


rkmap_str =<<"EOS"
a	あ		
i	い		
u	う		
e	え		
o	お		
ka	か		
ki	き		
ku	く		
ke	け		
ko	こ		
sa	さ		
si	し		
su	す		
se	せ		
so	そ		
ta	た		
ti	ち		
tu	つ		
te	て		
to	と		
na	な		
ni	に		
nu	ぬ		
ne	ね		
no	の		
ha	は		
hi	ひ		
hu	ふ		
he	へ		
ho	ほ		
ma	ま		
mi	み		
mu	む		
me	め		
mo	も		
ya	や		
yu	ゆ		
yo	よ		
ra	ら		
ri	り		
ru	る		
re	れ		
ro	ろ		
wa	わ		
wo	を		
nn	ん		
-	ー		
ga	が	か	濁音
gi	ぎ	き	濁音
gu	ぐ	く	濁音
ge	げ	け	濁音
go	ご	こ	濁音
za	ざ	さ	濁音
zi	じ	し	濁音
zu	ず	す	濁音
ze	ぜ	せ	濁音
zo	ぞ	そ	濁音
da	だ	た	濁音
di	ぢ	ち	濁音
du	づ	つ	濁音
de	で	て	濁音
do	ど	と	濁音
ba	ば	は	濁音
bi	び	ひ	濁音
bu	ぶ	ふ	濁音
be	べ	へ	濁音
bo	ぼ	ほ	濁音
vu	ゔ	う	濁音
pa	ぱ	は	半濁音
pi	ぴ	ひ	半濁音
pu	ぷ	ふ	半濁音
pe	ぺ	へ	半濁音
po	ぽ	ほ	半濁音
lya	ゃ	や	小
lyu	ゅ	ゆ	小
lyo	ょ	よ	小
la	ぁ	あ	小
li	ぃ	い	小
lu	ぅ	う	小
le	ぇ	え	小
lo	ぉ	お	小
lwa	ゎ	わ	小
ltu	っ		
sya	しゃ			
syu	しゅ			
syo	しょ			
zya	じゃ		
zyu	じゅ		
zyo	じょ		
kya	きゃ			
kyu	きゅ			
kyo	きょ			
gya	ぎゃ		
gyu	ぎゅ		
gyo	ぎょ		
tya	ちゃ		
tyu	ちゅ		
tyo	ちょ		
dya	ぢゃ		
dyu	ぢゅ		
dyo	ぢょ		
nya	にゃ		
nyu	にゅ		
nyo	にょ		
hya	ひゃ		
hyu	ひゅ		
hyo	ひょ		
bya	びゃ		
byu	びゅ		
byo	びょ		
pya	ぴゃ		
pyu	ぴゅ		
pyo	ぴょ		
mya	みゃ		
myu	みゅ		
myo	みょ		
rya	りゃ		
ryu	りゅ		
ryo	りょ		
thi	てぃ		半濁音
thu	てゅ		
dhi	でぃ		濁音
dhu	でゅ		濁音
toxu	とぅ		半濁音
doxu	どぅ		濁音
sye	しぇ		半濁音
tye	ちぇ		
zye	じぇ		濁音
dye	ぢぇ		濁音
fa	ふぁ		
fi	ふぃ		
fe	ふぇ		
fo	ふぉ		
fyu	ふゅ		
ixe	いぇ		半濁音
wi	うぃ		半濁音
we	うぇ		半濁音
uxo	うぉ		半濁音
va	ゔぁ		濁音
vi	ゔぃ		濁音
ve	ゔぇ		濁音
vo	ゔぉ		濁音
vuxyu	ゔゅ		濁音
kuxa	くぁ		半濁音
kuxi	くぃ		半濁音
kuxe	くぇ		半濁音
kuxo	くぉ		半濁音
kuxwa	くゎ		半濁音
guxa	ぐぁ		濁音
guxi	ぐぃ		濁音
guxe	ぐぇ		濁音
guxo	ぐぉ		濁音
guxwa	ぐゎ		濁音
tsa	つぁ		半濁音
tsi	つぃ		半濁音
tse	つぇ		半濁音
tso	つぉ		半濁音
,	、		
.	。		
tenn	…		
?	？		
/	・		
EOS
#/

#かなとローマ字の対応表
rkmap={}

#かなの一覧
rklist = rkmap_str.split(/\n/).map{|row|
    cols = row.split(/\t/)
    keys = cols.shift   #ローマ字
    kana = cols.shift   #かな
    cols.map!{|c| c!="" ? c : nil}
    src = cols.shift || nil #濁音などの元の文字
    mod = cols.shift || nil #濁音などの修飾
    rkmap[kana] = {:kana=>kana, :keys=>keys, :src=>src, :mod=>mod }
    kana
}


fingers={}
codes=[]
warn=[]
dup_tbl = Hash.new{|h,k|h[k]=[]}
rklist.each {|chr|
    info = rkmap[chr]
    mod = info[:mod]    #修飾：濁音、半濁音、小

    #入力文字の分解　　例：ぎゃ→[き,や]
    words = (info[:src] || info[:kana]).split("").map {|c|
        mod = mod || rkmap[c][:mod]
        while rkmap[c][:src]
            c = rkmap[c][:src]
        end
        c
    }

    first_pos = layout_map[words[0]] #最初の文字の位置
    if first_pos == nil
        warn.push("// W: #{chr} レイアウトに文字が存在しません")
        next
    end

    side = first_pos[:side]
    shift_type = first_pos[:shift]  #シフトの種類　0~3

    #修飾の調整：「小」は１文字のときだけ
    words.push(mod) if mod
    words.pop if words.size == 3 && mod == "小"

    #拗音の場合はシフトon/offどちらでも可
    is_verbose = 
        ((words.size > 1) && 
            (words[1] =~ /や|ゆ|よ/ || 
             (words[0] == "ふ" && words[1]=~/あ|い|え|お/) ||
             (words[0] == "ち" && words[1]== "え")
            )) ? true : false

    cols=Hash.new{|h,k|h[k]=[]} #同列チェック

    #入力文字をキーコードに変換　例：し→B_F
    first_word = words[0]
    words.map! {|w|
        p=layout_map[w] # 文字のレイアウト上の位置 :shift, :side, :row, :col
        if p 
            r,c=p[:row],p[:col]
            cols[c].push(kbd_map[r][c])
            kbd_map[r][c]
        else
            k=mod_key(w,side)
            if k == nil
                dk = layout_map[first_word][:next]
                dk = layout_map[dk]
                k = kbd_map[dk[:row]][dk[:col]]
            end
            cols[col_map[k]].push(k)
            k
        end
    }.uniq!

    #文字キーがシフトも兼ねている場合
    if SHIFT_KEYS.include?(words[0]) && words.size == 1
        # shift_type = first_pos[:shift] ? :must : :none 
    end

    #特殊なケース
    words = EXCEPTIONAL_KEY_SETS[chr] if EXCEPTIONAL_KEY_SETS.include?(chr)

    comment = INACTIVE.include?(chr) ? "//" : ""
    if is_verbose
        codes.push("#{comment}  {.key = #{words.join("|")}, .kana = \"#{info[:keys]}\"}, //#{chr} (シフト省略)")
    end

    SHIFT_ORDER[shift_type].each{|k| words.push(k) }
    codes.push("#{comment}  {.key = #{words.join("|")}, .kana = \"#{info[:keys]}\"}, //#{chr}")

    if comment == ""
        fingers[chr]= {
            :shift=>shift_type,
            :keys => words.map{|w|pos_map[w]},
        }
    end
    cols.each_value{|v| warn.push("// W: #{chr} 同じ列のキーがあります") if v.uniq.size>1 }
    dup_tbl[words.sort].push(chr)
}
dup_tbl.each{|key,val|
    if val.size>1
        warn.push("// E: #{val.join(", ")} キーの組み合わせが同一です")
    end
}

puts warn.join("\n")

if ARGV[0]== "test"
    #puts dup_tbl
    puts codes
    # puts layout_map
    # puts fingers.keys.map{|k|
    #     "#{k} #{fingers[k]}"
    # }
elsif ARGV[0]== "update"
    update_src(codes)
elsif ARGV[0]== "eval"
    #puts fingers
    eval_map(fingers)
end