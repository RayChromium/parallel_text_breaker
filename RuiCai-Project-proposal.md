# Project proposal  

## Project Goal  
This is actually a subset / tool I want to make for another bigger project of mine: **the goal is of this little project to take a large text file, split it into 1 KB blocks, and automatically identify semantically meaningful words and phrases by matching them against an offline Wikipedia titles database**. It will then annotate the text with reference numbers and compile a “References” section, pretty much like the reference number and entries in papers like thesis.  
My original plan of this tool I wanted was to generate a graph‑style view of interconnected concepts similar to tools like Obsidian or Logseq by adding "references" by searching on internet in a parallel manner ( for different block of the text ), but it's not realistic for the purpose of this course project because:
1. it would require too much network I/O which goes slow and unpredictable; 
2. I'm not so familiar with the Graph-view related sdks yet and I don't to spend the time to figure out yet, at least not during this project (because I don't think I have that much time anyways)  

Therefore I made the compromise by using a Wikipedia mirror dump database which only contains the titles since the size of this one is only like 100MB, whereas a database that contains page links is like 37GB and need to implement other database CRUD stuff that makes the whole project complicated. this should help me avoid network I/O, avoid database manipulation and avoid taking up too much space in my disk & memory.

## Use of Parallel Computing  
After reading some materials, turns out the [Aho-Corasick algorithm](https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm) fits pretty well for my purpose here, it matches multiple patterns in one given input text as long as the trie tree is constructed before the searching & matching stage. Assuming the patterns set P has a total length of $m = \sum_{i=0}^{len(P)}|title_i|$, then:  
1. ( run once ) Construction Phase: All patterns are inserted into the trie and failure links are built in a single‐threaded “setup” step. Once completed, the trie’s internal structure is immutable, time complexity would be: $O(m)$.
2. ( run in parallel ) Search Phase: Each thread independently traverses the read‑only trie on its assigned text chunk, keeping its own current state. As characters are fed through child and failure links, matches trigger callbacks but no trie nodes or links are ever modified, time complexity would be $O(n + k)$, where k is the total number of matches reported.
  
## Expected Outcome
I want to have a processed text file as the output, for example if a sentence in the input text file looks like this:  
```
ColdPlay had a tour concert in Helsinki in August, 2024.
```
The output is a JSON object embedding that same text under "text" and a "references" array of matches, e.g.:
```json
{
  "text": "ColdPlay had a tour concert in Helsinki in August, 2024.",
  "references": [
    { "start": 0, "end": 9, "titleIndex": 301, "title": "Coldplay" }
  ]
}
```
so if I need to develop it into a more advanced tool or feed it to other programs it would be easier.