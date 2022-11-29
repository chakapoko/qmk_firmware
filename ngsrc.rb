#あいえ-うお



LAYOUT = [
    [
        "へけてか★★するれ…",
        "せうのし、あいな。ー",
        "そもとこはくったり・",
    ], [
        "ぬらよわ★★えさひ小",
        "ゆまんをにちむんふつ",
        "やほめみねろきお★★",
    ]
    
        
]


LAYOUT = [
    [
        "へけてか★★するれ…",
        "せうのし、あいな。ー",
        "そもとこはくったり・",
    ], [
        "ぬらよわ★★えさひ小",
        "ゆまんをにちむんほつ",
        "★やめみねろきおふ★",
    ]    
]


LAYOUT = [
    [
        "へけてか★★するれ…",
        "そうのし、あいな。ー",
        "せもとこはくったり・",
    ], [
        "ぬらよめ★★えさひ小",
        "ゆまんをにちろんふつ",
        "やむわみね★きおほ★",
    ]
    
]

INACTIVE = [
    # "ふゅ", "つぉ"
]

CUT_RATE=0.5

SHIFT_KEYS = ["B_D", "B_K"]

#修飾キー
def mod_key(chr,w,side)
    mod_keys = {
        # "濁音"  => {:left=>"B_K", :right=>"B_D"},
        #"shift"   => {:left=>"B_J", :right=>"B_F"},
        "shift"  => {:left=>"B_K", :right=>"B_D"},
        "濁音"   => {:left=>"B_J", :right=>"B_F"},
        # "濁音" => {:left=>"B_H", :right=>"B_G"},
        # "半濁音" => {:left=>"B_N", :right=>"B_B"},
        "半濁音" => {:left=>"B_M", :right=>"B_V"},
        # "う" => {"濁音"=>"B_F", "半濁音"=>"B_V"},
        "う" => {"濁音"=>"B_G", "半濁音"=>"B_B"},    #うが右側
        "て" => {"濁音"=>"B_V", "半濁音"=>"B_G"},
    }
    if chr=~/ゔ.?/
        mod_keys["う"]["濁音"]
    elsif chr=~/う./
        mod_keys["う"]["半濁音"]
    # elsif chr=~/(ぜ|づ|ざ)/
    #     mod_keys["半濁音"][side]
    elsif chr=~/で./
        mod_keys["て"]["濁音"]
    elsif chr=~/て./
        mod_keys["て"]["半濁音"] 
    else
        mod_keys[w][side]
    end
end

#ソース
SRC=[
    "keyboards/dailycraft/claw44/keymaps/frym/naginata_frym.c",
    "keyboards/takashicompany/endzone34/keymaps/frym/naginata_frym.c"
]
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
    #評価
    key_count = Hash.new{|h,k|h[k]=0}
    reap = Hash.new{|h,k|h[k]=0}
    updown = Hash.new{|h,k|h[k]=0}
    same = Hash.new{|h,k|h[k]=0}
    arpegio = Hash.new{|h,k|h[k]=0}
    long_seq = Hash.new{|h,k|h[k]=0}
    long_buf = []
    cnt = 0
    sentences(fingers_map).each{|sentence|
        last_keys=nil
        sentence.each{|kana|
            keys = fingers_map[kana][:keys]
            has_reap = false
            has_same = false
            has_arp  = false
            has_updown = false
            if last_keys
                chars = last_keys[:kana]+kana
                keys.each{|k|
                    last_keys[:key].each{|lk|
                        if lk[:side]==k[:side]
                            row_d = (lk[:row]-k[:row]).abs
                            col_d = (lk[:finger]-k[:finger]).abs
                            has_reap = true if row_d == 2 && col_d == 0
                            has_same = true if row_d == 0 && col_d == 0
                            has_updown = true if row_d == 1 && col_d == 0
                            has_arp  = true if col_d ==1 && row_d < 2
                        end
                    }
                    key_count[k[:code]]+=1
                }
                reap[chars] += 1    if has_reap
                same[chars] += 1    if has_same
                updown[chars] += 1  if has_updown
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
    puts "■同鍵連続 #{same.keys.size}種 #{same.values.sum}回"
    puts cut(same,r)
    puts "■アルペジオ #{arpegio.keys.size}種 #{arpegio.values.sum}回"
    puts cut(arpegio,r)
    puts "■アルペジオ(3音以上) #{long_seq.keys.size}種 #{long_seq.values.sum}回"
    puts cut(long_seq,r/5.0)
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
            :code=>code,
            :side=>side, 
            :finger=>finger, 
            :extra=>extra,
            :row=>r,
        }
    }
}

def build_layout(board)
    shift=false
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
        shift=true
    }
    tbl.delete("★")
    tbl.each{|k,v|
        v[:other]=nil
        if !v[:shift]
            other = tbl.keys.filter{|x|tbl[x][:shift] && tbl[x][:row]==v[:row] && tbl[x][:col]==v[:col]}
            v[:other]= other[0] if other.size > 0
        end
    }
    tbl
end

LAYOUT_MAP = build_layout(LAYOUT)
#puts LAYOUT_MAP

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
    words.push(mod) if mod
    words.pop if words.size == 3 && mod == "小"
    shift_type = :none
    shift_type = mod==nil ? :must : :verbose if first_pos[:shift]
    shift_type = :verbose if shift_type == :none && first_pos[:other] == nil
    #words.push("shift") if first_pos[:shift] && mod==nil
    cols=Hash.new{|h,k|h[k]=0} #同列チェック
    # puts [words, shift_type].join(",")
    # puts mod_key(chr,'shift',side) if shift_type != :none
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
    if SHIFT_KEYS.include?(words[0]) && words.size == 1
        shift_type = first_pos[:shift] ? :must : :none 
    end
    comment = INACTIVE.include?(chr) ? "//" : ""
    if shift_type == :none || shift_type == :verbose
        codes.push("#{comment}  {.key = #{words.join("|")}, .kana = \"#{info[:keys]}\"}, //#{chr}")
    end
    if shift_type == :must || shift_type == :verbose
        shift = mod_key(chr,'shift',side)
        verbose_comments = SHIFT_KEYS
        .map{|s| shift==s ? "" : " (予備)"}
        .map{|s| (shift_type == :verbose ? "(冗長)" : "")+ s }
        m = chr=="ん" ? 0 : 1
        (0..m).each do |n|
           codes.push("#{comment}  {.key = #{[words,SHIFT_KEYS[n]].join("|")}, .kana = \"#{info[:keys]}\"}, //#{chr}#{verbose_comments[n]}")
        end
        words.push(shift) if shift_type == :must
    end
    if comment == ""
        fingers[chr]= {
            :shift=>shift_type!=:none,
            :keys => words.map{|w|pos_map[w]},
        }
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

if ARGV[0]== "test"
    #puts dup_tbl
    puts codes
    puts LAYOUT_MAP
    puts fingers.keys.map{|k|
        "#{k} #{fingers[k]}"
    }
elsif ARGV[0]== "update"
    update_src(codes)
elsif ARGV[0]== "eval"
    eval_map(fingers)
end
