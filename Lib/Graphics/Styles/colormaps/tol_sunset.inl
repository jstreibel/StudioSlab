auto tol_sunset = New<ColorMap>(
	"tol:sunset",
	ColorMap::Divergent,
	ColorSequence{
		Color(0.21176471, 0.29411765, 0.60392157, 1.00000000),
		Color(0.21484045, 0.30149942, 0.60838139, 1.00000000),
		Color(0.21791619, 0.30888120, 0.61284121, 1.00000000),
		Color(0.22099193, 0.31626298, 0.61730104, 1.00000000),
		Color(0.22406767, 0.32364475, 0.62176086, 1.00000000),
		Color(0.22714341, 0.33102653, 0.62622068, 1.00000000),
		Color(0.23021915, 0.33840830, 0.63068051, 1.00000000),
		Color(0.23329489, 0.34579008, 0.63514033, 1.00000000),
		Color(0.23637063, 0.35317186, 0.63960015, 1.00000000),
		Color(0.23944637, 0.36055363, 0.64405998, 1.00000000),
		Color(0.24252211, 0.36793541, 0.64851980, 1.00000000),
		Color(0.24559785, 0.37531719, 0.65297962, 1.00000000),
		Color(0.24867359, 0.38269896, 0.65743945, 1.00000000),
		Color(0.25174933, 0.39008074, 0.66189927, 1.00000000),
		Color(0.25482507, 0.39746251, 0.66635909, 1.00000000),
		Color(0.25790081, 0.40484429, 0.67081892, 1.00000000),
		Color(0.26097655, 0.41222607, 0.67527874, 1.00000000),
		Color(0.26405229, 0.41960784, 0.67973856, 1.00000000),
		Color(0.26712803, 0.42698962, 0.68419839, 1.00000000),
		Color(0.27020377, 0.43437140, 0.68865821, 1.00000000),
		Color(0.27327951, 0.44175317, 0.69311803, 1.00000000),
		Color(0.27635525, 0.44913495, 0.69757785, 1.00000000),
		Color(0.27943099, 0.45651672, 0.70203768, 1.00000000),
		Color(0.28250673, 0.46389850, 0.70649750, 1.00000000),
		Color(0.28558247, 0.47128028, 0.71095732, 1.00000000),
		Color(0.28865821, 0.47866205, 0.71541715, 1.00000000),
		Color(0.29296424, 0.48565936, 0.71933872, 1.00000000),
		Color(0.29850058, 0.49227220, 0.72272203, 1.00000000),
		Color(0.30403691, 0.49888504, 0.72610534, 1.00000000),
		Color(0.30957324, 0.50549789, 0.72948866, 1.00000000),
		Color(0.31510957, 0.51211073, 0.73287197, 1.00000000),
		Color(0.32064591, 0.51872357, 0.73625529, 1.00000000),
		Color(0.32618224, 0.52533641, 0.73963860, 1.00000000),
		Color(0.33171857, 0.53194925, 0.74302191, 1.00000000),
		Color(0.33725490, 0.53856209, 0.74640523, 1.00000000),
		Color(0.34279123, 0.54517493, 0.74978854, 1.00000000),
		Color(0.34832757, 0.55178777, 0.75317186, 1.00000000),
		Color(0.35386390, 0.55840062, 0.75655517, 1.00000000),
		Color(0.35940023, 0.56501346, 0.75993849, 1.00000000),
		Color(0.36493656, 0.57162630, 0.76332180, 1.00000000),
		Color(0.37047290, 0.57823914, 0.76670511, 1.00000000),
		Color(0.37600923, 0.58485198, 0.77008843, 1.00000000),
		Color(0.38154556, 0.59146482, 0.77347174, 1.00000000),
		Color(0.38708189, 0.59807766, 0.77685506, 1.00000000),
		Color(0.39261822, 0.60469050, 0.78023837, 1.00000000),
		Color(0.39815456, 0.61130334, 0.78362168, 1.00000000),
		Color(0.40369089, 0.61791619, 0.78700500, 1.00000000),
		Color(0.40922722, 0.62452903, 0.79038831, 1.00000000),
		Color(0.41476355, 0.63114187, 0.79377163, 1.00000000),
		Color(0.42029988, 0.63775471, 0.79715494, 1.00000000),
		Color(0.42583622, 0.64436755, 0.80053825, 1.00000000),
		Color(0.43137255, 0.65098039, 0.80392157, 1.00000000),
		Color(0.43783160, 0.65651672, 0.80699731, 1.00000000),
		Color(0.44429066, 0.66205306, 0.81007305, 1.00000000),
		Color(0.45074971, 0.66758939, 0.81314879, 1.00000000),
		Color(0.45720877, 0.67312572, 0.81622453, 1.00000000),
		Color(0.46366782, 0.67866205, 0.81930027, 1.00000000),
		Color(0.47012687, 0.68419839, 0.82237601, 1.00000000),
		Color(0.47658593, 0.68973472, 0.82545175, 1.00000000),
		Color(0.48304498, 0.69527105, 0.82852749, 1.00000000),
		Color(0.48950404, 0.70080738, 0.83160323, 1.00000000),
		Color(0.49596309, 0.70634371, 0.83467897, 1.00000000),
		Color(0.50242215, 0.71188005, 0.83775471, 1.00000000),
		Color(0.50888120, 0.71741638, 0.84083045, 1.00000000),
		Color(0.51534025, 0.72295271, 0.84390619, 1.00000000),
		Color(0.52179931, 0.72848904, 0.84698193, 1.00000000),
		Color(0.52825836, 0.73402537, 0.85005767, 1.00000000),
		Color(0.53471742, 0.73956171, 0.85313341, 1.00000000),
		Color(0.54117647, 0.74509804, 0.85620915, 1.00000000),
		Color(0.54763552, 0.75063437, 0.85928489, 1.00000000),
		Color(0.55409458, 0.75617070, 0.86236063, 1.00000000),
		Color(0.56055363, 0.76170704, 0.86543637, 1.00000000),
		Color(0.56701269, 0.76724337, 0.86851211, 1.00000000),
		Color(0.57347174, 0.77277970, 0.87158785, 1.00000000),
		Color(0.57993080, 0.77831603, 0.87466359, 1.00000000),
		Color(0.58638985, 0.78385236, 0.87773933, 1.00000000),
		Color(0.59284890, 0.78938870, 0.88081507, 1.00000000),
		Color(0.59930796, 0.79415609, 0.88342945, 1.00000000),
		Color(0.60576701, 0.79815456, 0.88558247, 1.00000000),
		Color(0.61222607, 0.80215302, 0.88773549, 1.00000000),
		Color(0.61868512, 0.80615148, 0.88988850, 1.00000000),
		Color(0.62514418, 0.81014994, 0.89204152, 1.00000000),
		Color(0.63160323, 0.81414840, 0.89419454, 1.00000000),
		Color(0.63806228, 0.81814687, 0.89634756, 1.00000000),
		Color(0.64452134, 0.82214533, 0.89850058, 1.00000000),
		Color(0.65098039, 0.82614379, 0.90065359, 1.00000000),
		Color(0.65743945, 0.83014225, 0.90280661, 1.00000000),
		Color(0.66389850, 0.83414072, 0.90495963, 1.00000000),
		Color(0.67035755, 0.83813918, 0.90711265, 1.00000000),
		Color(0.67681661, 0.84213764, 0.90926567, 1.00000000),
		Color(0.68327566, 0.84613610, 0.91141869, 1.00000000),
		Color(0.68973472, 0.85013456, 0.91357170, 1.00000000),
		Color(0.69619377, 0.85413303, 0.91572472, 1.00000000),
		Color(0.70265283, 0.85813149, 0.91787774, 1.00000000),
		Color(0.70911188, 0.86212995, 0.92003076, 1.00000000),
		Color(0.71557093, 0.86612841, 0.92218378, 1.00000000),
		Color(0.72202999, 0.87012687, 0.92433679, 1.00000000),
		Color(0.72848904, 0.87412534, 0.92648981, 1.00000000),
		Color(0.73494810, 0.87812380, 0.92864283, 1.00000000),
		Color(0.74140715, 0.88212226, 0.93079585, 1.00000000),
		Color(0.74786621, 0.88612072, 0.93294887, 1.00000000),
		Color(0.75432526, 0.89011918, 0.93510188, 1.00000000),
		Color(0.76078431, 0.89411765, 0.93725490, 1.00000000),
		Color(0.76693579, 0.89534794, 0.93187236, 1.00000000),
		Color(0.77308727, 0.89657824, 0.92648981, 1.00000000),
		Color(0.77923875, 0.89780854, 0.92110727, 1.00000000),
		Color(0.78539023, 0.89903883, 0.91572472, 1.00000000),
		Color(0.79154171, 0.90026913, 0.91034218, 1.00000000),
		Color(0.79769319, 0.90149942, 0.90495963, 1.00000000),
		Color(0.80384468, 0.90272972, 0.89957709, 1.00000000),
		Color(0.80999616, 0.90396002, 0.89419454, 1.00000000),
		Color(0.81614764, 0.90519031, 0.88881200, 1.00000000),
		Color(0.82229912, 0.90642061, 0.88342945, 1.00000000),
		Color(0.82845060, 0.90765090, 0.87804691, 1.00000000),
		Color(0.83460208, 0.90888120, 0.87266436, 1.00000000),
		Color(0.84075356, 0.91011150, 0.86728181, 1.00000000),
		Color(0.84690504, 0.91134179, 0.86189927, 1.00000000),
		Color(0.85305652, 0.91257209, 0.85651672, 1.00000000),
		Color(0.85920800, 0.91380238, 0.85113418, 1.00000000),
		Color(0.86535948, 0.91503268, 0.84575163, 1.00000000),
		Color(0.87151096, 0.91626298, 0.84036909, 1.00000000),
		Color(0.87766244, 0.91749327, 0.83498654, 1.00000000),
		Color(0.88381392, 0.91872357, 0.82960400, 1.00000000),
		Color(0.88996540, 0.91995386, 0.82422145, 1.00000000),
		Color(0.89611688, 0.92118416, 0.81883891, 1.00000000),
		Color(0.90226836, 0.92241446, 0.81345636, 1.00000000),
		Color(0.90841984, 0.92364475, 0.80807382, 1.00000000),
		Color(0.91457132, 0.92487505, 0.80269127, 1.00000000),
		Color(0.91918493, 0.92410611, 0.79500192, 1.00000000),
		Color(0.92226067, 0.92133795, 0.78500577, 1.00000000),
		Color(0.92533641, 0.91856978, 0.77500961, 1.00000000),
		Color(0.92841215, 0.91580161, 0.76501346, 1.00000000),
		Color(0.93148789, 0.91303345, 0.75501730, 1.00000000),
		Color(0.93456363, 0.91026528, 0.74502115, 1.00000000),
		Color(0.93763937, 0.90749712, 0.73502499, 1.00000000),
		Color(0.94071511, 0.90472895, 0.72502884, 1.00000000),
		Color(0.94379085, 0.90196078, 0.71503268, 1.00000000),
		Color(0.94686659, 0.89919262, 0.70503652, 1.00000000),
		Color(0.94994233, 0.89642445, 0.69504037, 1.00000000),
		Color(0.95301807, 0.89365629, 0.68504421, 1.00000000),
		Color(0.95609381, 0.89088812, 0.67504806, 1.00000000),
		Color(0.95916955, 0.88811995, 0.66505190, 1.00000000),
		Color(0.96224529, 0.88535179, 0.65505575, 1.00000000),
		Color(0.96532103, 0.88258362, 0.64505959, 1.00000000),
		Color(0.96839677, 0.87981546, 0.63506344, 1.00000000),
		Color(0.97147251, 0.87704729, 0.62506728, 1.00000000),
		Color(0.97454825, 0.87427912, 0.61507113, 1.00000000),
		Color(0.97762399, 0.87151096, 0.60507497, 1.00000000),
		Color(0.98069973, 0.86874279, 0.59507882, 1.00000000),
		Color(0.98377547, 0.86597463, 0.58508266, 1.00000000),
		Color(0.98685121, 0.86320646, 0.57508651, 1.00000000),
		Color(0.98992695, 0.86043829, 0.56509035, 1.00000000),
		Color(0.99300269, 0.85767013, 0.55509419, 1.00000000),
		Color(0.99607843, 0.85490196, 0.54509804, 1.00000000),
		Color(0.99592464, 0.84890427, 0.53940792, 1.00000000),
		Color(0.99577086, 0.84290657, 0.53371780, 1.00000000),
		Color(0.99561707, 0.83690888, 0.52802768, 1.00000000),
		Color(0.99546328, 0.83091119, 0.52233756, 1.00000000),
		Color(0.99530950, 0.82491349, 0.51664744, 1.00000000),
		Color(0.99515571, 0.81891580, 0.51095732, 1.00000000),
		Color(0.99500192, 0.81291811, 0.50526720, 1.00000000),
		Color(0.99484814, 0.80692042, 0.49957709, 1.00000000),
		Color(0.99469435, 0.80092272, 0.49388697, 1.00000000),
		Color(0.99454056, 0.79492503, 0.48819685, 1.00000000),
		Color(0.99438677, 0.78892734, 0.48250673, 1.00000000),
		Color(0.99423299, 0.78292964, 0.47681661, 1.00000000),
		Color(0.99407920, 0.77693195, 0.47112649, 1.00000000),
		Color(0.99392541, 0.77093426, 0.46543637, 1.00000000),
		Color(0.99377163, 0.76493656, 0.45974625, 1.00000000),
		Color(0.99361784, 0.75893887, 0.45405613, 1.00000000),
		Color(0.99346405, 0.75294118, 0.44836601, 1.00000000),
		Color(0.99331027, 0.74694348, 0.44267589, 1.00000000),
		Color(0.99315648, 0.74094579, 0.43698577, 1.00000000),
		Color(0.99300269, 0.73494810, 0.43129566, 1.00000000),
		Color(0.99284890, 0.72895040, 0.42560554, 1.00000000),
		Color(0.99269512, 0.72295271, 0.41991542, 1.00000000),
		Color(0.99254133, 0.71695502, 0.41422530, 1.00000000),
		Color(0.99238754, 0.71095732, 0.40853518, 1.00000000),
		Color(0.99223376, 0.70495963, 0.40284506, 1.00000000),
		Color(0.99161861, 0.69788543, 0.39792388, 1.00000000),
		Color(0.99054210, 0.68973472, 0.39377163, 1.00000000),
		Color(0.98946559, 0.68158401, 0.38961938, 1.00000000),
		Color(0.98838908, 0.67343329, 0.38546713, 1.00000000),
		Color(0.98731257, 0.66528258, 0.38131488, 1.00000000),
		Color(0.98623606, 0.65713187, 0.37716263, 1.00000000),
		Color(0.98515955, 0.64898116, 0.37301038, 1.00000000),
		Color(0.98408304, 0.64083045, 0.36885813, 1.00000000),
		Color(0.98300654, 0.63267974, 0.36470588, 1.00000000),
		Color(0.98193003, 0.62452903, 0.36055363, 1.00000000),
		Color(0.98085352, 0.61637832, 0.35640138, 1.00000000),
		Color(0.97977701, 0.60822760, 0.35224913, 1.00000000),
		Color(0.97870050, 0.60007689, 0.34809689, 1.00000000),
		Color(0.97762399, 0.59192618, 0.34394464, 1.00000000),
		Color(0.97654748, 0.58377547, 0.33979239, 1.00000000),
		Color(0.97547097, 0.57562476, 0.33564014, 1.00000000),
		Color(0.97439446, 0.56747405, 0.33148789, 1.00000000),
		Color(0.97331795, 0.55932334, 0.32733564, 1.00000000),
		Color(0.97224145, 0.55117263, 0.32318339, 1.00000000),
		Color(0.97116494, 0.54302191, 0.31903114, 1.00000000),
		Color(0.97008843, 0.53487120, 0.31487889, 1.00000000),
		Color(0.96901192, 0.52672049, 0.31072664, 1.00000000),
		Color(0.96793541, 0.51856978, 0.30657439, 1.00000000),
		Color(0.96685890, 0.51041907, 0.30242215, 1.00000000),
		Color(0.96578239, 0.50226836, 0.29826990, 1.00000000),
		Color(0.96470588, 0.49411765, 0.29411765, 1.00000000),
		Color(0.96086121, 0.48412149, 0.28950404, 1.00000000),
		Color(0.95701653, 0.47412534, 0.28489043, 1.00000000),
		Color(0.95317186, 0.46412918, 0.28027682, 1.00000000),
		Color(0.94932718, 0.45413303, 0.27566321, 1.00000000),
		Color(0.94548251, 0.44413687, 0.27104960, 1.00000000),
		Color(0.94163783, 0.43414072, 0.26643599, 1.00000000),
		Color(0.93779316, 0.42414456, 0.26182238, 1.00000000),
		Color(0.93394848, 0.41414840, 0.25720877, 1.00000000),
		Color(0.93010381, 0.40415225, 0.25259516, 1.00000000),
		Color(0.92625913, 0.39415609, 0.24798155, 1.00000000),
		Color(0.92241446, 0.38415994, 0.24336794, 1.00000000),
		Color(0.91856978, 0.37416378, 0.23875433, 1.00000000),
		Color(0.91472511, 0.36416763, 0.23414072, 1.00000000),
		Color(0.91088043, 0.35417147, 0.22952710, 1.00000000),
		Color(0.90703576, 0.34417532, 0.22491349, 1.00000000),
		Color(0.90319108, 0.33417916, 0.22029988, 1.00000000),
		Color(0.89934641, 0.32418301, 0.21568627, 1.00000000),
		Color(0.89550173, 0.31418685, 0.21107266, 1.00000000),
		Color(0.89165705, 0.30419070, 0.20645905, 1.00000000),
		Color(0.88781238, 0.29419454, 0.20184544, 1.00000000),
		Color(0.88396770, 0.28419839, 0.19723183, 1.00000000),
		Color(0.88012303, 0.27420223, 0.19261822, 1.00000000),
		Color(0.87627835, 0.26420607, 0.18800461, 1.00000000),
		Color(0.87243368, 0.25420992, 0.18339100, 1.00000000),
		Color(0.86858900, 0.24421376, 0.17877739, 1.00000000),
		Color(0.86236063, 0.23452518, 0.17593233, 1.00000000),
		Color(0.85374856, 0.22514418, 0.17485582, 1.00000000),
		Color(0.84513649, 0.21576317, 0.17377932, 1.00000000),
		Color(0.83652441, 0.20638216, 0.17270281, 1.00000000),
		Color(0.82791234, 0.19700115, 0.17162630, 1.00000000),
		Color(0.81930027, 0.18762015, 0.17054979, 1.00000000),
		Color(0.81068820, 0.17823914, 0.16947328, 1.00000000),
		Color(0.80207612, 0.16885813, 0.16839677, 1.00000000),
		Color(0.79346405, 0.15947712, 0.16732026, 1.00000000),
		Color(0.78485198, 0.15009612, 0.16624375, 1.00000000),
		Color(0.77623991, 0.14071511, 0.16516724, 1.00000000),
		Color(0.76762784, 0.13133410, 0.16409073, 1.00000000),
		Color(0.75901576, 0.12195309, 0.16301423, 1.00000000),
		Color(0.75040369, 0.11257209, 0.16193772, 1.00000000),
		Color(0.74179162, 0.10319108, 0.16086121, 1.00000000),
		Color(0.73317955, 0.09381007, 0.15978470, 1.00000000),
		Color(0.72456747, 0.08442907, 0.15870819, 1.00000000),
		Color(0.71595540, 0.07504806, 0.15763168, 1.00000000),
		Color(0.70734333, 0.06566705, 0.15655517, 1.00000000),
		Color(0.69873126, 0.05628604, 0.15547866, 1.00000000),
		Color(0.69011918, 0.04690504, 0.15440215, 1.00000000),
		Color(0.68150711, 0.03752403, 0.15332564, 1.00000000),
		Color(0.67289504, 0.02814302, 0.15224913, 1.00000000),
		Color(0.66428297, 0.01876201, 0.15117263, 1.00000000),
		Color(0.65567090, 0.00938101, 0.15009612, 1.00000000),
		Color(0.64705882, 0.00000000, 0.14901961, 1.00000000),
	}
);