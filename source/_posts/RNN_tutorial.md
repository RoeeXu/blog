---
title: RNN_tutorial
date: 2017-09-13 9:45:56
tags: rnn
---

<script type="text/javascript"
   src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML">
</script>


FORWARD FROM http://www.wildml.com/2015/09/recurrent-neural-networks-tutorial-part-1-introduction-to-rnns/ BY DENNY BRITZ

## Recurrent Neural Networks Tutorial, Part 1 – Introduction to RNNs

Recurrent Neural Networks (RNNs) are popular models that have shown great promise in many NLP tasks. But despite their recent popularity I’ve only found a limited number of resources that throughly explain how RNNs work, and how to implement them. That’s what this tutorial is about. It’s a multi-part series in which I’m planning to cover the following:

<!--more-->

+ Introduction to RNNs (this post)
+ [Implementing a RNN using Python and Theano](http://www.wildml.com/2015/09/recurrent-neural-networks-tutorial-part-2-implementing-a-language-model-rnn-with-python-numpy-and-theano/)
+ [Understanding the Backpropagation Through Time (BPTT) algorithm and the vanishing gradient problem](http://www.wildml.com/2015/10/recurrent-neural-networks-tutorial-part-3-backpropagation-through-time-and-vanishing-gradients/)
+ [Implementing a GRU/LSTM RNN](http://www.wildml.com/2015/10/recurrent-neural-network-tutorial-part-4-implementing-a-grulstm-rnn-with-python-and-theano/)

As part of the tutorial we will implement a [recurrent neural network based language model](http://www.fit.vutbr.cz/research/groups/speech/publi/2010/mikolov_interspeech2010_IS100722.pdf). The applications of language models are two-fold: First, it allows us to score arbitrary sentences based on how likely they are to occur in the real world. This gives us a measure of grammatical and semantic correctness. Such models are typically used as part of Machine Translation systems. Secondly, a language model allows us to generate new text (I think that’s the much cooler application). Training a language model on Shakespeare allows us to generate Shakespeare-like text. [This fun post](http://karpathy.github.io/2015/05/21/rnn-effectiveness/) by Andrej Karpathy demonstrates what character-level language models based on RNNs are capable of.

I’m assuming that you are somewhat familiar with basic Neural Networks. If you’re not, you may want to head over to [Implementing A Neural Network From Scratch](http://www.wildml.com/2015/09/implementing-a-neural-network-from-scratch/),  which guides you through the ideas and implementation behind non-recurrent networks.

## What are RNNs?

The idea behind RNNs is to make use of sequential information. In a traditional neural network we assume that all inputs (and outputs) are independent of each other. But for many tasks that’s a very bad idea. If you want to predict the next word in a sentence you better know which words came before it. RNNs are called recurrent because they perform the same task for every element of a sequence, with the output being depended on the previous computations. Another way to think about RNNs is that they have a “memory” which captures information about what has been calculated so far. In theory RNNs can make use of information in arbitrarily long sequences, but in practice they are limited to looking back only a few steps (more on this later). Here is what a typical RNN looks like:

![A recurrent neural network and the unfolding in time of the computation involved in its forward computation. Source: Nature](/md_pic/rnn_1.jpg "A recurrent neural network and the unfolding in time of the computation involved in its forward computation. Source: Nature")

The above diagram shows a RNN being unrolled (or unfolded) into a full network. By unrolling we simply mean that we write out the network for the complete sequence. For example, if the sequence we care about is a sentence of 5 words, the network would be unrolled into a 5-layer neural network, one layer for each word. The formulas that govern the computation happening in a RNN are as follows:

+ \\( x_t \\) is the input at time step \\( t \\). For example, \\( x_1 \\) could be a one-hot vector corresponding to the second word of a sentence.
+ \\( s_t \\) is the hidden state at time step \\( t \\). It’s the “memory” of the network. \\( s_t \\) is calculated based on the previous hidden state and the input at the current step: \\( s_t=f(Ux_t + Ws_{t-1}) \\). The function \\( f \\) usually is a nonlinearity such as [tanh](https://reference.wolfram.com/language/ref/Tanh.html) or [ReLU](https://en.wikipedia.org/wiki/Rectifier_(neural_networks)).  \\( s_{-1} \\), which is required to calculate the first hidden state, is typically initialized to all zeroes.
+ \\( o_t \\) is the output at step \\( t \\). For example, if we wanted to predict the next word in a sentence it would be a vector of probabilities across our vocabulary. \\( o_t = \mathrm{softmax}(Vs_t) \\).

There are a few things to note here:

+ You can think of the hidden state \\( s_t \\) as the memory of the network. \\( s_t \\) captures information about what happened in all the previous time steps. The output at step \\( o_t \\) is calculated solely based on the memory at time \\( t \\). As briefly mentioned above, it’s a bit more complicated  in practice because \\( s_t \\) typically can’t capture information from too many time steps ago.
+ Unlike a traditional deep neural network, which uses different parameters at each layer, a RNN shares the same parameters (\\( U, V, W \\) above) across all steps. This reflects the fact that we are performing the same task at each step, just with different inputs. This greatly reduces the total number of parameters we need to learn.
+ The above diagram has outputs at each time step, but depending on the task this may not be necessary. For example, when predicting the sentiment of a sentence we may only care about the final output, not the sentiment after each word. Similarly, we may not need inputs at each time step. The main feature of an RNN is its hidden state, which captures some information about a sequence.

## What can RNNs do?

RNNs have shown great success in many NLP tasks. At this point I should mention that the most commonly used type of RNNs are [LSTMs](https://en.wikipedia.org/wiki/Long_short_term_memory), which are much better at capturing long-term dependencies than vanilla RNNs are. But don’t worry, LSTMs are essentially the same thing as the RNN we will develop in this tutorial, they just have a different way of computing the hidden state. We’ll cover LSTMs in more detail in a later post. Here are some example applications of RNNs in NLP (by non means an exhaustive list).

### Language Modeling and Generating Text

Given a sequence of words we want to predict the probability of each word given the previous words. Language Models allow us to measure how likely a sentence is, which is an important input for Machine Translation (since high-probability sentences are typically correct). A side-effect of being able to predict the next word is that we get a generative model, which allows us to generate new text by sampling from the output probabilities. And depending on what our training data is we can generate [all kinds of stuff](http://karpathy.github.io/2015/05/21/rnn-effectiveness/). In Language Modeling our input is typically a sequence of words (encoded as one-hot vectors for example), and our output is the sequence of predicted words. When training the network we set \\( o_t = x_{t+1} \\) since we want the output at step \\( t \\) to be the actual next word.

Research papers about Language Modeling and Generating Text:

+ [Recurrent neural network based language model](http://www.fit.vutbr.cz/research/groups/speech/publi/2010/mikolov_interspeech2010_IS100722.pdf)
+ [Extensions of Recurrent neural network based language model](http://www.fit.vutbr.cz/research/groups/speech/publi/2011/mikolov_icassp2011_5528.pdf)
+ [Generating Text with Recurrent Neural Networks](http://machinelearning.wustl.edu/mlpapers/paper_files/ICML2011Sutskever_524.pdf)

### Machine Translation

Machine Translation is similar to language modeling in that our input is a sequence of words in our source language (e.g. German). We want to output a sequence of words in our target language (e.g. English). A key difference is that our output only starts after we have seen the complete input, because the first word of our translated sentences may require information captured from the complete input sequence.

![RNN for Machine Translation](/md_pic/rnn_2.png "RNN for Machine Translation. Image Source: http://cs224d.stanford.edu/lectures/CS224d-Lecture8.pdf")

Research papers about Machine Translation:

+ [A Recursive Recurrent Neural Network for Statistical Machine Translation](http://www.aclweb.org/anthology/P14-1140.pdf)
+ [Sequence to Sequence Learning with Neural Networks](http://papers.nips.cc/paper/5346-sequence-to-sequence-learning-with-neural-networks.pdf)
+ [Joint Language and Translation Modeling with Recurrent Neural Networks](http://research.microsoft.com/en-us/um/people/gzweig/Pubs/EMNLP2013RNNMT.pdf)

### Speech Recognition

Given an input sequence of acoustic signals from a sound wave, we can predict a sequence of phonetic segments together with their probabilities.

Research papers about Speech Recognition:

+ [Towards End-to-End Speech Recognition with Recurrent Neural Networks](http://www.jmlr.org/proceedings/papers/v32/graves14.pdf)

### Generating Image Descriptions

Together with convolutional Neural Networks, RNNs have been used as part of a model to [generate descriptions](http://cs.stanford.edu/people/karpathy/deepimagesent/) for unlabeled images. It’s quite amazing how well this seems to work. The combined model even aligns the generated words with features found in the images.

![Deep Visual-Semantic Alignments for Generating Image Descriptions](/md_pic/rnn_3.png "Deep Visual-Semantic Alignments for Generating Image Descriptions. Source: http://cs.stanford.edu/people/karpathy/deepimagesent/")

## Training RNNs

Training a RNN is similar to training a traditional Neural Network. We also use the backpropagation algorithm, but with a little twist. Because the parameters are shared by all time steps in the network, the gradient at each output depends not only on the calculations of the current time step, but also the previous time steps. For example, in order to calculate the gradient at t=4 we would need to backpropagate 3 steps and sum up the gradients. This is called Backpropagation Through Time (BPTT). If this doesn’t make a whole lot of sense yet, don’t worry, we’ll have a whole post on the gory details. For now, just be aware of the fact that vanilla RNNs trained with BPTT [have difficulties](http://www.jmlr.org/proceedings/papers/v28/pascanu13.pdf) learning long-term dependencies (e.g. dependencies between steps that are far apart) due to what is called the vanishing/exploding gradient problem. There exists some machinery to deal with these problems, and certain types of RNNs (like LSTMs) were specifically designed to get around them.

## RNN Extensions

Over the years researchers have developed more sophisticated types of RNNs to deal with some of the shortcomings of the vanilla RNN model. We will cover them in more detail in a later post, but I want this section to serve as a brief overview so that you are familiar with the taxonomy of models.

**Bidirectional RNNs** are based on the idea that the output at time t may not only depend on the previous elements in the sequence, but also future elements. For example, to predict a missing word in a sequence you want to look at both the left and the right context. Bidirectional RNNs are quite simple. They are just two RNNs stacked on top of each other. The output is then computed based on the hidden state of both RNNs.

![Bidirectional RNN](/md_pic/rnn_4.png "Bidirectional RNN")

**Deep (Bidirectional) RNN** are similar to Bidirectional RNNs, only that we now have multiple layers per time step. In practice this gives us a higher learning capacity (but we also need a lot of training data).

![Deep (Bidirectional) RNN](/md_pic/rnn_5.png "Deep (Bidirectional) RNN")

**LSTM networks** are quite popular these days and we briefly talked about them above. LSTMs don’t have a fundamentally different architecture from RNNs, but they use a different function to compute the hidden state. The memory in LSTMs are called cells and you can think of them as black boxes that take as input the previous state \\(h_{t-1}\\) and current input \\(x_t\\). Internally these cells  decide what to keep in (and what to erase from) memory. They then combine the previous state, the current memory, and the input. It turns out that these types of units are very efficient at capturing long-term dependencies. LSTMs can be quite confusing in the beginning but if you’re interested in learning more [this post has an excellent explanation](http://colah.github.io/posts/2015-08-Understanding-LSTMs/).

## Conclusion

So far so good. I hope you’ve gotten a basic understanding of what RNNs are and what they can do. In the next post we’ll implement a first version of our language model RNN using Python and Theano. Please leave questions in the comments!