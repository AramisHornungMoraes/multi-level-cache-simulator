/*
AHM Engine
Copyright © 2013 Aramis Hornung Moraes.

AHM's files contained within the
distributed product are copyright © Aramis Hornung Moraes.

The contents of the AHM Engine distribution archive may not be redistributed,
reproduced, modified, transmitted, broadcast, published or adapted in any
way, shape or form, without the prior written consent of the owner,
Aramis Hornung Moraes.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


If you do not accept all the terms of the license then you do not have
the right to use this software, such use is illegal
under the terms of international Copyright law.
*/

#ifndef LIST_H
#define LIST_H
typedef struct node
{
	int data;
	struct node *next;
	struct node *prev;
}node;

typedef struct list
{
	struct node *first;
	struct node *last;
}list;

// List Manager
struct node *AllocateNode();
struct list *AllocateList();
int IsListEmpty(struct list *l);

// Object nodes maneger
void InsertFirst(struct list *l, int data);
void InsertAfter(struct list *l, struct node *rn, int data);
void InsertLast(struct list *l, int data);
int RemoveFirst(struct list *l);
int RemoveLast(struct list *l);
int RemoveAfter(struct list *l, struct node *n);
void PrintList(struct list *l);
void CleanList(list *l);
#endif /*#define LIST_H*/
