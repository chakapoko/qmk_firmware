
LAYOUT = [
    [
        "小★してやみする★★",
        "★★もかよーいう★★",
        "★★こはゆったり★★",
    ], [
        "★★★をせわつち★★",
        "★★◎、ふまれ★★★",
        "★★★らそへなく★★",
    ], [
        "★★にひめろあ★★★",
        "★★★のさえ。◎★★",
        "★★きとけほお★★★",
    ], [
        "★★★★★★★★★★",
        "★★ん・…★ねん★★",
        "★★★む★★ぬ★★★",
    ]
    
]


INACTIVE = [
    "ふぃ","ふぇ","つぁ"
]

CUT_RATE=0.5

#修飾キー
def mod_key(chr,w,side)
    mod_keys = {
        # "濁音"  => {:left=>"B_K", :right=>"B_D"},
        #"shift"   => {:left=>"B_J", :right=>"B_F"},
        "shift1"  => {:left=>"B_D", :right=>"B_D"},
        "shift2"  => {:left=>"B_K", :right=>"B_K"},
        "濁音"   => {:left=>"B_J", :right=>"B_F"},
        #"半濁音" => {:left=>"B_COMM", :right=>"B_C"},
        # "半濁音" => {:left=>"B_H", :right=>"B_G"},
        "半濁音" => {:left=>"B_L", :right=>"B_S"},
        #"半濁音" => {:left=>"B_M", :right=>"B_V"},
        #"う" => {"濁音"=>"B_H", "半濁音"=>"B_N"},
        "う" => {"濁音"=>"B_S", "半濁音"=>"B_X"},
    }
    # if chr=~/ゔ/
    #      mod_keys["濁音"][:right]
    # elsif chr=~/う./
    #     mod_keys["半濁音"][:right]
    if chr=~/ゔ.?/
        mod_keys["う"]["濁音"]
    elsif chr=~/う./
        mod_keys["う"]["半濁音"]
    elsif chr=~/で./
        mod_keys["半濁音"][:right]
    elsif chr=~/どぅ/
        "B_M"
    # elsif chr=~/て./
    #     "B_G"
    else
        mod_keys[w][side]
    end
end

#ソース
SRC="keyboards/dailycraft/claw44/keymaps/frym/naginata_frym.c"
def update_src(codes_array)
    src=[]
    is_src=true
    f = File.open(SRC,"r")
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
    f = File.open(SRC,"w")
    f.puts(src)
    f.close
end

#評価
TEXT="../src-kana.txt"
def test_map(fingers_map)
    f = File.open(TEXT,"r")
    sentence_list = []
    sentence = []
    #読み込み
    while line=f.gets
        chars = line.split("")
        while chars.size > 0
            c= chars.slice(0,2).join("")
            if fingers_map[c]
                sentence.push(c)
                chars.slice!(0,2)
            else
                c=chars[0]
                if fingers_map[c]
                    sentence.push(c)
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
    #評価
    reap = Hash.new{|h,k|h[k]=0}
    updown = Hash.new{|h,k|h[k]=0}
    same = Hash.new{|h,k|h[k]=0}
    arpegio = Hash.new{|h,k|h[k]=0}
    long_seq = Hash.new{|h,k|h[k]=0}
    long_buf = []
    cnt = 0
    sentence_list.each{|sentence|
        last_keys=nil
        sentence.each{|kana|
            keys = fingers_map[kana]
            has_reap = false
            has_updown = false
            has_same = false
            has_arp  = false
            if last_keys
                chars = last_keys[:kana]+kana
                last_keys[:key].each{|lk|
                    keys.each{|k|
                        if lk[:side]==k[:side]
                            row_d = (lk[:row]-k[:row]).abs
                            col_d = (lk[:finger]-k[:finger]).abs
                            has_reap = true if row_d == 2 && col_d == 0
                            has_updown = true if row_d == 1 && col_d == 0
                            has_same = true if row_d == 0 && col_d == 0
                            has_arp  = true if col_d ==1 && row_d < 2
                        end
                    }
                }
                reap[chars] += 1    if has_reap
                updown[chars] += 1    if has_updown
                same[chars] += 1    if has_same
                arpegio[chars] += 1 if has_arp
            end
            if long_buf.size==0 || has_arp || has_same
                long_buf.push(kana)
            else
                long_seq[long_buf.join("")]+=1 if long_buf.size>2
                long_buf = []
            end
            last_keys= {:kana=>kana, :key=>keys}
            cnt+=1
        }
        long_seq[long_buf.join("")]+=1 if long_buf.size>2
        long_buf = []
    }
    r=CUT_RATE
    puts "■計 #{cnt}回"
    puts "■同指跳躍 #{reap.keys.size}種 #{reap.values.sum}回"
    puts cut(reap,r)
    puts "■同指連接 #{updown.keys.size}種 #{updown.values.sum}回"
    puts cut(updown,r)
    puts "■同指同段 #{same.keys.size}種 #{same.values.sum}回"
    puts cut(same,r)
    puts "■アルペジオ #{arpegio.keys.size}種 #{arpegio.values.sum}回"
    puts cut(arpegio,r)
    puts "■アルペジオ(3音以上) #{long_seq.keys.size}種 #{long_seq.values.sum}回"
    puts cut(long_seq,r/5.0)
end

def cut(h,rate)
    max = h.values.sum * rate
    sum = 0
    res = {}
    h.keys.sort{|a,b|h[b]<=>h[a]}.each {|k|
        res[k]=h[k] if sum < max
        sum += h[k]
    }
    res
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
finger_index = [5,4,3,2,2,2,2,3,4,5]
kbd_map.each_index{|r|
    row = kbd_map[r]
    row.each_index{|c|
        code=row[c]
        side = c < 5 ? "left" : "right"
        finger = finger_index[c]
        extra = c==4 || c==5
        pos_map[code]={
            :side=>side, 
            :finger=>finger, 
            :extra=>extra,
            :row=>r,
        }
    }
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
    tbl
end

LAYOUT_MAP = build_layout(LAYOUT)

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
tye	ちぇ		半濁音
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
/	・		
EOS
#/

rkmap={}
rklist = rkmap_str.split(/\n/).map{|row|
    cols = row.split(/\t/)
    keys = cols.shift
    kana = cols.shift
    cols.map!{|c| c!="" ? c : nil}
    src = cols.shift || nil
    mod = cols.shift || nil
    # mod = "半濁音" if mod==nil && kana=~/[ぁぃぅぇぉ]/
    rkmap[kana] = {:kana=>kana, :keys=>keys, :src=>src, :mod=>mod }
    kana
}


fingers={}
codes=[]
warn=[]
dup_tbl = Hash.new{|h,k|h[k]=[]}
rklist.each {|chr|
    info = rkmap[chr]
    puts info
    side = info[:side]
    mod = info[:mod]
    words = (info[:src] || info[:kana]).split("").map {|c|
        #puts rkmap[c]
        mod = mod || rkmap[c][:mod]
        #puts mod
        while rkmap[c][:src]
            c = rkmap[c][:src]
            #puts rkmap[c]
        end
        c
    }
    first_pos = LAYOUT_MAP[words[0]]
    if first_pos == nil
        warn.push("// W: #{chr} レイアウトに文字が存在しません")
        next
    end
    #puts info
    #puts first_pos
    side = first_pos[:side]
    #puts mod
    mod = nil if words.size == 2 && mod == "小"
    words.push(mod) if mod
    sft = first_pos[:shift]
    words.push("shift1") if sft==1 || sft==3
    words.push("shift2") if sft==2 || sft==3
    cols=Hash.new{|h,k|h[k]=0} #同列チェック
    words.map! {|w|
        p=LAYOUT_MAP[w] 
        if p 
            r,c=p[:row],p[:col]
            cols[c]+=1
            kbd_map[r][c]
        else
            k=mod_key(chr,w,side)
            cols[col_map[k]]+=1
            k
        end
    }.uniq!
    comment = INACTIVE.include?(chr) ? "//" : ""
    codes.push("#{comment}  {.key = #{words.join("|")}, .kana = \"#{info[:keys]}\"}, //#{chr}")
    if comment == ""
        fingers[chr]= words.map{|w|pos_map[w]}
    end
    cols.each_value{|v| warn.push("// W: #{chr} 同じ列のキーがあります") if v>1 }
    dup_tbl[words.sort].push(chr)
}
dup_tbl.each{|key,val|
    if val.size>1
        warn.push("// E: #{val.join(", ")} キーの組み合わせが同一です")
    end
}

puts warn.join("\n")

if ARGV[0]== "update"
    update_src(codes)
elsif ARGV[0]== "test"
    test_map(fingers)
else
    #puts dup_tbl
    #puts  codes
end
