* FUNCTION input
  1:     ST  3,-1(1)	Store return address 
  2:     IN  2,2,2	Grab int input 
  3:     LD  3,-1(1)	Load return address 
  4:     LD  1,0(1)	Adjust fp 
  5:    JMP  7,0(3)	Return 
* END FUNCTION input
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION output
  6:     ST  3,-1(1)	Store return address 
  7:     LD  3,-2(1)	Load parameter 
  8:    OUT  3,3,3	Output integer 
  9:     LD  3,-1(1)	Load return address 
 10:     LD  1,0(1)	Adjust fp 
 11:    JMP  7,0(3)	Return 
* END FUNCTION output
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION inputb
 12:     ST  3,-1(1)	Store return address 
 13:    INB  2,2,2	Grab bool input 
 14:     LD  3,-1(1)	Load return address 
 15:     LD  1,0(1)	Adjust fp 
 16:    JMP  7,0(3)	Return 
* END FUNCTION inputb
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION outputb
 17:     ST  3,-1(1)	Store return address 
 18:     LD  3,-2(1)	Load parameter 
 19:   OUTB  3,3,3	Output bool 
 20:     LD  3,-1(1)	Load return address 
 21:     LD  1,0(1)	Adjust fp 
 22:    JMP  7,0(3)	Return 
* END FUNCTION outputb
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION inputc
 23:     ST  3,-1(1)	Store return address 
 24:    INC  2,2,2	Grab char input 
 25:     LD  3,-1(1)	Load return address 
 26:     LD  1,0(1)	Adjust fp 
 27:    JMP  7,0(3)	Return 
* END FUNCTION inputc
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION outputc
 28:     ST  3,-1(1)	Store return address 
 29:     LD  3,-2(1)	Load parameter 
 30:   OUTC  3,3,3	Output char 
 31:     LD  3,-1(1)	Load return address 
 32:     LD  1,0(1)	Adjust fp 
 33:    JMP  7,0(3)	Return 
* END FUNCTION outputc
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION outnl
 34:     ST  3,-1(1)	Store return address 
 35:  OUTNL  3,3,3	Output a newline 
 36:     LD  3,-1(1)	Load return address 
 37:     LD  1,0(1)	Adjust fp 
 38:    JMP  7,0(3)	Return 
* END FUNCTION outnl
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION main
* TOFF set: -2
 39:     ST  3,-1(1)	Store return address 
* COMPOUND
* TOFF set: -2
* Compound Body
* CALL output
 40:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
 41:    LDC  3,10(6)	Load constant 
 42:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
 43:    LDC  3,1(6)	Load constant 
 44:     LD  4,-4(1)	load result of left 
 45:    DIV  5,4,3	%:integer divide 
 46:    MUL  5,5,3	%:* 
 47:    SUB  3,4,5	%:- 
 48:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
 49:    LDA  1,-2(1)	Ghost frame becomes new active frame 
 50:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
 51:    JMP  7,-46(7)	CALL output
 52:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
 53:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
 54:    LDC  3,10(6)	Load constant 
 55:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
 56:    LDC  3,2(6)	Load constant 
 57:     LD  4,-4(1)	load result of left 
 58:    DIV  5,4,3	%:integer divide 
 59:    MUL  5,5,3	%:* 
 60:    SUB  3,4,5	%:- 
 61:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
 62:    LDA  1,-2(1)	Ghost frame becomes new active frame 
 63:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
 64:    JMP  7,-59(7)	CALL output
 65:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
 66:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
 67:    LDC  3,10(6)	Load constant 
 68:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
 69:    LDC  3,3(6)	Load constant 
 70:     LD  4,-4(1)	load result of left 
 71:    DIV  5,4,3	%:integer divide 
 72:    MUL  5,5,3	%:* 
 73:    SUB  3,4,5	%:- 
 74:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
 75:    LDA  1,-2(1)	Ghost frame becomes new active frame 
 76:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
 77:    JMP  7,-72(7)	CALL output
 78:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
 79:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
 80:    LDC  3,10(6)	Load constant 
 81:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
 82:    LDC  3,4(6)	Load constant 
 83:     LD  4,-4(1)	load result of left 
 84:    DIV  5,4,3	%:integer divide 
 85:    MUL  5,5,3	%:* 
 86:    SUB  3,4,5	%:- 
 87:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
 88:    LDA  1,-2(1)	Ghost frame becomes new active frame 
 89:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
 90:    JMP  7,-85(7)	CALL output
 91:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
 92:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
 93:    LDC  3,10(6)	Load constant 
 94:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
 95:    LDC  3,5(6)	Load constant 
 96:     LD  4,-4(1)	load result of left 
 97:    DIV  5,4,3	%:integer divide 
 98:    MUL  5,5,3	%:* 
 99:    SUB  3,4,5	%:- 
100:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
101:    LDA  1,-2(1)	Ghost frame becomes new active frame 
102:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
103:    JMP  7,-98(7)	CALL output
104:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL outnl
105:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
106:    LDA  1,-2(1)	Ghost frame becomes new active frame 
107:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
108:    JMP  7,-75(7)	CALL outnl
109:    LDA  3,0(2)	Save the result in ac 
* Call end outnl
* CALL output
110:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
111:    LDC  3,10(6)	Load constant 
112:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
113:    LDC  3,10(6)	Load constant 
114:     LD  4,-4(1)	load result of left 
115:    DIV  5,4,3	%:integer divide 
116:    MUL  5,5,3	%:* 
117:    SUB  3,4,5	%:- 
118:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
119:    LDA  1,-2(1)	Ghost frame becomes new active frame 
120:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
121:    JMP  7,-116(7)	CALL output
122:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
123:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
124:    LDC  3,10(6)	Load constant 
125:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
126:    LDC  3,11(6)	Load constant 
127:     LD  4,-4(1)	load result of left 
128:    DIV  5,4,3	%:integer divide 
129:    MUL  5,5,3	%:* 
130:    SUB  3,4,5	%:- 
131:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
132:    LDA  1,-2(1)	Ghost frame becomes new active frame 
133:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
134:    JMP  7,-129(7)	CALL output
135:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL outnl
136:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
137:    LDA  1,-2(1)	Ghost frame becomes new active frame 
138:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
139:    JMP  7,-106(7)	CALL outnl
140:    LDA  3,0(2)	Save the result in ac 
* Call end outnl
* CALL output
141:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
142:    LDC  3,0(6)	Load constant 
143:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
144:    LDC  3,1(6)	Load constant 
145:     LD  4,-4(1)	load result of left 
146:    DIV  5,4,3	%:integer divide 
147:    MUL  5,5,3	%:* 
148:    SUB  3,4,5	%:- 
149:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
150:    LDA  1,-2(1)	Ghost frame becomes new active frame 
151:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
152:    JMP  7,-147(7)	CALL output
153:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL outnl
154:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
155:    LDA  1,-2(1)	Ghost frame becomes new active frame 
156:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
157:    JMP  7,-124(7)	CALL outnl
158:    LDA  3,0(2)	Save the result in ac 
* Call end outnl
* CALL output
159:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
160:    LDC  3,10(6)	Load constant 
161:    LDC  4,-1(0)	load -1 
162:    MUL  3,3,4	* by -1 
163:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
164:    LDC  3,1(6)	Load constant 
165:     LD  4,-4(1)	load result of left 
166:    DIV  5,4,3	%:integer divide 
167:    MUL  5,5,3	%:* 
168:    SUB  3,4,5	%:- 
169:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
170:    LDA  1,-2(1)	Ghost frame becomes new active frame 
171:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
172:    JMP  7,-167(7)	CALL output
173:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
174:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
175:    LDC  3,10(6)	Load constant 
176:    LDC  4,-1(0)	load -1 
177:    MUL  3,3,4	* by -1 
178:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
179:    LDC  3,2(6)	Load constant 
180:     LD  4,-4(1)	load result of left 
181:    DIV  5,4,3	%:integer divide 
182:    MUL  5,5,3	%:* 
183:    SUB  3,4,5	%:- 
184:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
185:    LDA  1,-2(1)	Ghost frame becomes new active frame 
186:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
187:    JMP  7,-182(7)	CALL output
188:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
189:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
190:    LDC  3,10(6)	Load constant 
191:    LDC  4,-1(0)	load -1 
192:    MUL  3,3,4	* by -1 
193:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
194:    LDC  3,3(6)	Load constant 
195:     LD  4,-4(1)	load result of left 
196:    DIV  5,4,3	%:integer divide 
197:    MUL  5,5,3	%:* 
198:    SUB  3,4,5	%:- 
199:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
200:    LDA  1,-2(1)	Ghost frame becomes new active frame 
201:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
202:    JMP  7,-197(7)	CALL output
203:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
204:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
205:    LDC  3,10(6)	Load constant 
206:    LDC  4,-1(0)	load -1 
207:    MUL  3,3,4	* by -1 
208:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
209:    LDC  3,4(6)	Load constant 
210:     LD  4,-4(1)	load result of left 
211:    DIV  5,4,3	%:integer divide 
212:    MUL  5,5,3	%:* 
213:    SUB  3,4,5	%:- 
214:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
215:    LDA  1,-2(1)	Ghost frame becomes new active frame 
216:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
217:    JMP  7,-212(7)	CALL output
218:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
219:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
220:    LDC  3,10(6)	Load constant 
221:    LDC  4,-1(0)	load -1 
222:    MUL  3,3,4	* by -1 
223:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
224:    LDC  3,5(6)	Load constant 
225:     LD  4,-4(1)	load result of left 
226:    DIV  5,4,3	%:integer divide 
227:    MUL  5,5,3	%:* 
228:    SUB  3,4,5	%:- 
229:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
230:    LDA  1,-2(1)	Ghost frame becomes new active frame 
231:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
232:    JMP  7,-227(7)	CALL output
233:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL outnl
234:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
235:    LDA  1,-2(1)	Ghost frame becomes new active frame 
236:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
237:    JMP  7,-204(7)	CALL outnl
238:    LDA  3,0(2)	Save the result in ac 
* Call end outnl
* CALL output
239:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
240:    LDC  3,10(6)	Load constant 
241:    LDC  4,-1(0)	load -1 
242:    MUL  3,3,4	* by -1 
243:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
244:    LDC  3,10(6)	Load constant 
245:     LD  4,-4(1)	load result of left 
246:    DIV  5,4,3	%:integer divide 
247:    MUL  5,5,3	%:* 
248:    SUB  3,4,5	%:- 
249:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
250:    LDA  1,-2(1)	Ghost frame becomes new active frame 
251:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
252:    JMP  7,-247(7)	CALL output
253:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
254:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
255:    LDC  3,10(6)	Load constant 
256:    LDC  4,-1(0)	load -1 
257:    MUL  3,3,4	* by -1 
258:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
259:    LDC  3,11(6)	Load constant 
260:     LD  4,-4(1)	load result of left 
261:    DIV  5,4,3	%:integer divide 
262:    MUL  5,5,3	%:* 
263:    SUB  3,4,5	%:- 
264:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
265:    LDA  1,-2(1)	Ghost frame becomes new active frame 
266:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
267:    JMP  7,-262(7)	CALL output
268:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL outnl
269:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
270:    LDA  1,-2(1)	Ghost frame becomes new active frame 
271:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
272:    JMP  7,-239(7)	CALL outnl
273:    LDA  3,0(2)	Save the result in ac 
* Call end outnl
* CALL output
274:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
275:    LDC  3,331(6)	Load constant 
276:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
277:    LDC  3,31(6)	Load constant 
278:     LD  4,-4(1)	load result of left 
279:    DIV  5,4,3	%:integer divide 
280:    MUL  5,5,3	%:* 
281:    SUB  3,4,5	%:- 
282:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
283:    LDA  1,-2(1)	Ghost frame becomes new active frame 
284:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
285:    JMP  7,-280(7)	CALL output
286:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
287:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
288:    LDC  3,331(6)	Load constant 
289:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
290:    LDC  3,31(6)	Load constant 
291:    LDC  4,-1(0)	load -1 
292:    MUL  3,3,4	* by -1 
293:     LD  4,-4(1)	load result of left 
294:    DIV  5,4,3	%:integer divide 
295:    MUL  5,5,3	%:* 
296:    SUB  3,4,5	%:- 
297:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
298:    LDA  1,-2(1)	Ghost frame becomes new active frame 
299:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
300:    JMP  7,-295(7)	CALL output
301:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
302:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
303:    LDC  3,331(6)	Load constant 
304:    LDC  4,-1(0)	load -1 
305:    MUL  3,3,4	* by -1 
306:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
307:    LDC  3,31(6)	Load constant 
308:     LD  4,-4(1)	load result of left 
309:    DIV  5,4,3	%:integer divide 
310:    MUL  5,5,3	%:* 
311:    SUB  3,4,5	%:- 
312:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
313:    LDA  1,-2(1)	Ghost frame becomes new active frame 
314:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
315:    JMP  7,-310(7)	CALL output
316:    LDA  3,0(2)	Save the result in ac 
* Call end output
* CALL output
317:     ST  1,-2(1)	Store fp in ghost frame for output 
* TOFF dec: -3
* TOFF dec: -5
* Param
318:    LDC  3,331(6)	Load constant 
319:    LDC  4,-1(0)	load -1 
320:    MUL  3,3,4	* by -1 
321:     ST  3,-4(1)	store result of left 
* TOFF dec: -5
322:    LDC  3,31(6)	Load constant 
323:    LDC  4,-1(0)	load -1 
324:    MUL  3,3,4	* by -1 
325:     LD  4,-4(1)	load result of left 
326:    DIV  5,4,3	%:integer divide 
327:    MUL  5,5,3	%:* 
328:    SUB  3,4,5	%:- 
329:     ST  3,-4(1)	Push parameter 
* Param end
* TOFF dec: -6
330:    LDA  1,-2(1)	Ghost frame becomes new active frame 
331:    LDA  3,1(7)	Return address in ac 
* TOFF: -2
332:    JMP  7,-327(7)	CALL output
333:    LDA  3,0(2)	Save the result in ac 
* Call end output
* TOFF set: -2
* END COMPOUND
* Add standard closing in case there is no return statement
334:    LDC  2,0(6)	Set return value to 0 
335:     LD  3,-1(1)	Load return address 
336:     LD  1,0(1)	Adjust fp 
337:    JMP  7,0(3)	Return 
* END FUNCTION main
* 
* ** ** ** ** ** ** ** ** ** ** ** **
  0:    JMP  7,337(7)	Jump to init [backpatch] 
* INIT
338:    LDA  1,0(0)	set first frame at end of globals 
339:     ST  1,0(1)	store old fp (point to self) 
* INIT GLOBALS AND STATICS
* END INIT GLOBALS AND STATICS
340:    LDA  3,1(7)	Return address in ac 
341:    JMP  7,-303(7)	Jump to main 
342:   HALT  0,0,0	DONE! 
* END INIT
