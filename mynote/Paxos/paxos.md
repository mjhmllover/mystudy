> ***在一个分布式数据库系统中，如果各节点的初始状态一致，每个节点都执行相同的操作序列，那么他们最后能得到一个一致的状态。为保证每个节点执行相同的命令序列，需要在每一条指令上执行一个“一致性算法”以保证每个节点看到的指令一致***

> **P1**：An acceptor must accept the first proposal that it receives.

> **P2**：If a proposal with value v is chosen, then every higher-numbered proposal that is chosen has value v.

> **P2a**：If a proposal with value v is chosen, then every higher-numbered proposal accepted by any acceptor has value v.

> **P2b**：If a proposal with value v is chosen, then every higher-numbered proposal issued by any proposer has value v.

>**P2c**：For any v and n, if a proposal with value v and number n is issued, then there is a set S consisting of a majority of acceptors such that either
>(a) no acceptor in S has accepted any proposal numbered less than n, or
>(b) v is the value of the highest-numbered proposal among all proposals numbered less than n accepted by the acceptors in S.
>
>如果一个编号为n的proposal提交value v，要想被接受就不能随便提交一个value，就必须是一个受限制的value.
>
>如果前面有value v选出了，那以后就提交这个value v；否则proposer决定提交哪个value，具体做法就是事前咨询，事中决定，事后提交，也就是说可以通过消息传递模型实现。
>
>只要在proposer提交提案前，咨询一下acceptor，看他们的最高编号是啥，他们是否选择了某个value v，再根据acceptor的回答进行选择新的编号、value提交，就可以满足P2c。

> 总结：P1是P2的不变式，P2>>P2a>>P2b>>P2c逐次增强。 

# 1. 算法背景

​		Paxos算法是Lamport宗师提出的一种基于消息传递的分布式一致性算法，使其获得2013年图灵奖。

​		Paxos由Lamport于1998年在《The Part-Time Parliament》论文中首次公开，最初的描述使用希腊的一个小岛Paxos作为比喻，描述了Paxos小岛中通过决议的流程，并以此命名这个算法，但是这个描述理解起来比较有挑战性。后来在2001年，Lamport觉得同行不能理解他的幽默感，于是重新发表了朴实的算法描述版本《Paxos Made Simple》。

​		自Paxos问世以来就持续垄断了分布式一致性算法，Paxos这个名词几乎等同于分布式一致性。Google的很多大型分布式系统都采用了Paxos算法来解决分布式一致性问题，如Chubby、Megastore以及Spanner等。开源的ZooKeeper，以及MySQL 5.7推出的用来取代传统的主从复制的MySQL Group Replication等纷纷采用Paxos算法解决分布式一致性问题。

​		然而，Paxos的最大特点就是难，不仅难以理解，更难以实现。

